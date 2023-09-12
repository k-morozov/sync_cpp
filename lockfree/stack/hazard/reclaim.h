#pragma once

#include <atomic>
#include <functional>
#include <algorithm>

#include "hp_owner.h"

namespace sync_cpp {

struct Reclaim {
    struct data_to_reclaim {
        void* data;
        std::function<void(void*)> deleter;
        data_to_reclaim* next;

        template<class T>
        explicit data_to_reclaim(T* p):
            data(p),
            deleter(&do_delete<T>),
            next(nullptr)
        {}

        ~data_to_reclaim() {
            deleter(data);
        }
    };

    std::atomic<data_to_reclaim*> nodes_to_reclaim_;

    void add_to_reclaim_list(data_to_reclaim* node) {
        node->next = nodes_to_reclaim_.load();
        while(!nodes_to_reclaim_.compare_exchange_weak(node->next, node)) {}
    }

    template<typename T>
    void reclaim_later(T* data) {
		add_to_reclaim_list(new data_to_reclaim(data));
    }

    void delete_nodes_with_no_hazard() {
        data_to_reclaim* current = nodes_to_reclaim_.exchange(nullptr);
        while (current) {
            data_to_reclaim* next = current->next;

            if (!OutstandingHazardPtrsFor(current->data)) {
                delete current;
            } else {
				add_to_reclaim_list(current);
            }

            current = next;
        }
    }

    static bool OutstandingHazardPtrsFor(void* p) {
		return std::ranges::any_of(hazard_ptrs, [p](HazardPtr& hp){
			return hp.ptr.load() == p;
		});
    }

    static std::atomic<void*>& GetHazardPtrForCurrentThread() {
        thread_local static hp_owner hazard;
        return hazard.get_pointer();
    }
};

} // namespace sync_cpp