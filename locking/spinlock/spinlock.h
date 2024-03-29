//
// Created by focus on 8/11/23.
//

#pragma once

#include <atomic>
#include <memory>
#include <thread>

namespace sync_cpp {

template<typename T>
class Spinlock final : private T {
public:
	void lock() {
		T::acquire();
	}

	void unlock() {
		T::release();
	}
};

class SimplePolicy {
protected:
	void acquire() {
		while (is_lock_.exchange(true)) {}
	}

	void release() {
		is_lock_.store(false);
	}

private:
	std::atomic<bool> is_lock_{false};
};

class ReadablePolicy {
protected:
	void acquire() {
		while (is_lock_.exchange(true)) {
			while(is_lock_.load()) {
				std::this_thread::yield();
			}
		}
	}

	void release() {
		is_lock_.store(false);
	}

private:
	std::atomic<bool> is_lock_{false};
};

class MMReadablePolicy {
protected:
	void acquire() {
		while (is_lock_.exchange(true,
//								 std::memory_order_acquire
								std::memory_order_acq_rel
								 )) {
			while(is_lock_.load(std::memory_order_relaxed)) {
				std::this_thread::yield();
			}
		}
	}

	void release() {
		is_lock_.store(false, std::memory_order_release);
	}

private:
	std::atomic<bool> is_lock_{false};
};



} // namespace sync_cpp
