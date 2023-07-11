#pragma once

#include <atomic>

#include <functional>
#include <memory>
#include <thread>

#include "hp_owner.h"

namespace sync_cpp {

template <typename T>
class LockFreeStackHazard final {
    struct Node {
        std::shared_ptr<T> data;
        Node* next;
    };
    struct data_to_reclame;

    public:
        void Push(T data) {
            Node* node = new Node{
                std::make_shared<T>(std::move(data)),
                nullptr
            };
            while(!head_.compare_exchange_weak(node->next, node)) {}
        }

        std::shared_ptr<T> TryPop() {
            std::atomic<void*>& hp = GetHazardPtrForCurrentThread();
            Node* old_head = head_.load();
            do {
                Node* temp;
                do {
                    temp = old_head;
                    hp.store(old_head);
                    old_head = head_.load();
                } while (old_head != temp);
            } while (old_head && !head_.compare_exchange_strong(old_head, old_head->next));
            hp.store(nullptr);

            std::shared_ptr<T> res;
            if (old_head) {
                res.swap(old_head->data);
                if (OutstandingHazardPtrsFor(old_head)) {
                    reclaim_later(old_head);
                } else {
                    delete old_head;
                }
                delete_nodes_with_no_hazard();
            }
            return res;
        }

        ~LockFreeStackHazard() {
            while(TryPop()) {}
        }

    private:
        std::atomic<Node*> head_;

        std::atomic<data_to_reclame*> nodes_to_reclame_;

        struct data_to_reclame {
            void* data;
            std::function<void(void*)> deleter;
            data_to_reclame* next;

            template<class U>
            data_to_reclame(U* p):
                data(p),
                deleter(&do_delete<U>),
                next(nullptr)
            {}

            ~data_to_reclame() {
                deleter(data);
            }
        };
        
        std::atomic<void*>& GetHazardPtrForCurrentThread() {
            thread_local static hp_owner hazard;
            return hazard.get_pointer();
        }

        bool OutstandingHazardPtrsFor(void* p) {
            for(size_t i=0; i<MaxHazardPointers; i++) {
                if (hazard_ptrs[i].ptr.load() == p) {
                    return true;
                }
            }
            return false;
        }
        
        void add_to_reclame_list(data_to_reclame* node) {
            node->next = nodes_to_reclame_.load();
            while(!nodes_to_reclame_.compare_exchange_weak(node->next, node)) {}
        }

        template<typename U>
        void reclaim_later(U* data) {
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
};

} // namespace sync_cpp