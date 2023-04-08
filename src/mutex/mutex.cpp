//
// Created by focus on 4/8/23.
//
#include "mutex.h"

#include <atomic>

#include <common/sys_waiter.h>

namespace sync_cpp {

class Mutex::mutex_impl {
public:
	using UInt = uint32_t;

	enum class State : UInt {
		Free = 0,
		Locking
	};

	~mutex_impl() = default;

	void lock() {
		waiters_.fetch_add(1);
		while (state_.exchange(State::Locking) == State::Locking) {
			sys_waiter::futex_wait(cast(), static_cast<UInt>(State::Locking));
		}
		waiters_.fetch_sub(-1);
	}

	void unlock() {
		state_.store(State::Free);
		if (waiters_ > 0) {
			sys_waiter::futex_wake(cast(), 1);
		}
	}

private:
	std::atomic<State> state_{State::Free};
	std::atomic<size_t> waiters_{0};

private:
	[[nodiscard]]
	UInt * cast() {
		return reinterpret_cast<UInt *>(&state_);
	}
};

Mutex::Mutex()
	: impl_ (std::make_unique<mutex_impl>())
{}

Mutex::~Mutex() = default;

void Mutex::lock() {
	impl_->lock();
}

void Mutex::unlock() {
	impl_->unlock();
}

} // namespace sync_cpp

