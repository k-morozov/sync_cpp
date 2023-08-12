//
// Created by focus on 8/12/23.
//

#pragma once

#include <atomic>
#include <memory>
#include <thread>

namespace sync_cpp {

class QueueSpinlock final {
public:
	class Guard final {
	public:
		friend QueueSpinlock;

		explicit Guard(QueueSpinlock& s) : host_(s) {
			host_.Acquire(this);
		}

		~Guard() {
			host_.Release(this);
		}

	private:
		QueueSpinlock& host_;
		std::atomic<Guard*> next_{nullptr};
		std::atomic<bool> is_owner_{false};

		void set_next(Guard* next) {
			next_.store(next);
		}

		void set_owner() {
			is_owner_.store(true, std::memory_order_release);
		}

		bool has_next() {
			return next_.load() != nullptr;
		}

		void set_next_owner() {
			next_.load()->set_owner();
		}

		bool is_owner() {
			return is_owner_.load(std::memory_order_acquire);
		}
	};

protected:
	void Acquire(Guard* waiter) {
		auto old_tail = tail_.exchange(waiter);
		if (old_tail != nullptr) {
			old_tail->set_next(waiter);
			while (!waiter->is_owner()) {
				std::this_thread::yield();
			}
		} else {
			waiter->set_owner();
		}
	}
	void Release(Guard* owner) {
		Guard* self;
		do {
			if (owner->has_next()) {
				owner->set_next_owner();
				return;
			}
			self = owner;
		} while(!tail_.compare_exchange_weak(self, nullptr));
	}

private:
	std::atomic<Guard*> tail_{nullptr};
};

} // namespace sync_cpp