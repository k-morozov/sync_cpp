#pragma once

#include <atomic>
#include <functional>

#include "hp_owner.h"

namespace sync_cpp {

struct Reclaim {
    struct data_to_reclame {
        void* data;
        std::function<void(void*)> deleter;
        data_to_reclame* next;

        template<class T>
        data_to_reclame(T* p):
            data(p),
            deleter(&do_delete<T>),
            next(nullptr)
        {}

        ~data_to_reclame() {
            deleter(data);
        }
    };

    std::atomic<data_to_reclame*> nodes_to_reclame_;

    void add_to_reclame_list(data_to_reclame* node) {
        node->next = nodes_to_reclame_.load();
        while(!nodes_to_reclame_.compare_exchange_weak(node->next, node)) {}
    }

    template<typename T>
    void reclaim_later(T* data) {
        add_to_reclame_list(new data_to_reclame(data));
    }

    void delete_nodes_with_no_hazard() {
        data_to_reclame* current = nodes_to_reclame_.exchange(nullptr);
        while (current) {
            data_to_reclame* next = current->next;

            if (!OutstandingHazardPtrsFor(current->data)) {
                delete current;
            } else {
                add_to_reclame_list(current);
            }

            current = next;
        }
    }

    bool OutstandingHazardPtrsFor(void* p) {
        for(size_t i=0; i<MaxHazardPointers; i++) {
            if (hazard_ptrs[i].ptr.load() == p) {
                return true;
            }
        }
        return false;
    }

    std::atomic<void*>& GetHazardPtrForCurrentThread() {
        thread_local static hp_owner hazard;
        return hazard.get_pointer();
    }
};

} // namespace sync_cpp