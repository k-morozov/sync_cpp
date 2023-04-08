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

	enum State : UInt {
		Free = 0,
		Lock,
		Waiters
	};

	~mutex_impl() = default;

	void lock() {
		State old_state = cas(State::Free, State::Lock);
		if (old_state == State::Free) {
			return;
		}
		do {
			if (old_state == State::Waiters
				|| cas(State::Lock, State::Waiters) == State::Lock) {
				sys_waiter::futex_wait(cast_to_uint(), State::Waiters);
			}
		} while ((old_state=cas(State::Lock, State::Waiters)) != State::Free);
	}

	void unlock() {
		if (unlock_with_contention() == State::Waiters) {
			sys_waiter::futex_wake(cast_to_uint(), 1);
		}
	}

private:
	std::atomic<UInt> state_{State::Free};

private:
	[[nodiscard]]
	UInt * cast_to_uint() {
		return reinterpret_cast<UInt *>(&state_);
	}

	State cas(UInt expected, const UInt desire) {
		state_.compare_exchange_weak(expected, desire);
		return static_cast<State>(expected);
	}

	State unlock_with_contention() {
		return static_cast<State>(state_.exchange(State::Free));
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

