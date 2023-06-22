//
// Created by focus on 6/20/23.
//

#pragma once

#include <atomic>
#include <memory>

namespace sync_cpp {

template<typename T>
class LockFreeStackCounting final {
	struct ControlBlock;

	struct Node {
		std::shared_ptr<T> data;
		ControlBlock next;
		std::atomic<int> internal_index;
	};

	struct ControlBlock {
		int external_index{0};
		Node* data{nullptr};
	};

public:
	void Push(T data) {
		ControlBlock new_block{
			.external_index=1,
			.data=new Node{
				.data=std::make_shared<T>(std::move(data)),
				.next=top_.load(),
				.internal_index=0
			}
		};

		while (!top_.compare_exchange_weak(new_block.data->next, new_block)) {};
	}

	std::shared_ptr<T> TryPop() {
		ControlBlock old_top = top_.load();
		while (true) {
			IncreaseExternalIndex(old_top);
			Node* const p = old_top.data;
			if (!p) {
				return {};
			}

			if (top_.compare_exchange_weak(old_top, p->next)) {
				std::shared_ptr<T> data;
				data.swap(p->data);
				const int diff_index = old_top.external_index - 2;
				if (p->internal_index.fetch_add(diff_index) == -diff_index) {
					delete p;
				}
				return data;
			} else {
				if (p->internal_index.fetch_sub(1) == 1) {
					delete p;
				}
			}
		}
	}

	~LockFreeStackCounting() {
		while (TryPop()) {}
	}

private:
	std::atomic<ControlBlock> top_{};

	void IncreaseExternalIndex(ControlBlock& old_top) {
		ControlBlock old_top_with_increased_index{};
		do {
			old_top_with_increased_index = old_top;
			++old_top_with_increased_index.external_index;
		} while (!top_.compare_exchange_weak(old_top, old_top_with_increased_index));
		old_top.external_index = old_top_with_increased_index.external_index;
	}
};

} // namespace sync_cpp