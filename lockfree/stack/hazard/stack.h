#pragma once

#include <atomic>

#include <functional>
#include <memory>
#include <thread>

#include "reclaim.h"

namespace sync_cpp {

template <typename T>
class LockFreeStackHazard final : private Reclaim {
    struct Node {
        std::shared_ptr<T> data;
        Node* next;
    };

	std::atomic<Node*> head_;

	Node* DropHead() {
		std::atomic<void*>& hp = GetHazardPtrForCurrentThread();
		Node* old_head = head_.load();
		do {
			Node* temp;
			do {
				temp = old_head;
				hp.store(old_head);
				old_head = head_.load();
			} while (old_head != temp);
			// hp sets to head

		} while (old_head && !head_.compare_exchange_strong(old_head, old_head->next));

		// clean hp
		hp.store(nullptr);

		return old_head;
	}

    public:
        void Push(T data) {
            Node* node = new Node{
                .data=std::make_shared<T>(std::move(data)),
                .next=nullptr
            };
            while(!head_.compare_exchange_weak(node->next, node)) {}
        }

        std::shared_ptr<T> TryPop() {
			Node* old_head = DropHead();

            std::shared_ptr<T> res;
            if (old_head) {
                res.swap(old_head->data);
//                if (OutstandingHazardPtrsFor(old_head)) {
//                    reclaim_later(old_head);
//                } else {
//                    delete old_head;
//                }
//                delete_nodes_with_no_hazard();
            }
            return res;
        }

        ~LockFreeStackHazard() {
            while(TryPop()) {}
        }
};

} // namespace sync_cpp