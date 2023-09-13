//
// Created by focus on 9/13/23.
//

#include <atomic>
#include <memory>


namespace sync_cpp {

template<class T>
class SPSCQueue final {
	struct Node final {
		std::shared_ptr<T> data{};
		Node* next{nullptr};
	};

	std::atomic<Node*> head_;
	std::atomic<Node*> tail_;

	Node* pop_head() {
		Node* const old_head = head_.load();
		if (old_head == tail_.load()) {
			return nullptr;
		}
		head_.store(old_head->next);
		return old_head;
	}

public:
	SPSCQueue() : head_(new Node), tail_(head_.load())
	{}

	SPSCQueue(const SPSCQueue&) = delete;
	SPSCQueue(SPSCQueue&&) noexcept = delete;
	~SPSCQueue() {
		while (pop() != nullptr);
		delete head_;
	}

	void push(T new_value) {
		auto new_data = std::make_shared<T>(std::move(new_value));
		Node* p = new Node;
		Node* const old_tail = tail_.load();
		old_tail->data.swap(new_data);
		old_tail->next = p;
		tail_.store(p);
	}

	std::shared_ptr<T> pop() {
		Node* old_head = pop_head();
		if (!old_head) {
			return std::shared_ptr<T>();
		}

		std::shared_ptr<T> res(old_head->data);
		delete old_head;
		return res;
	}
};

} // namespace sync_cpp
