//
// Created by focus on 8/11/23.
//

#pragma once

#include <atomic>

#include "futex_waiter.h"

namespace sync_cpp {

class SimpleMutex final {
	enum Status : uint32_t {
		Unlock,
		Lock,
		LockContention
	};

public:
	void lock() {
		Status old = cas(Unlock, Lock);
		if (old == Unlock) {
			return;
		}

		do {
			park();
		} while ((old=cas(Unlock, LockContention)) != Unlock);
	}

	void unlock() {
		if (unlock_has_contention()) {
			impl::wake(cast());
		}
	}

private:
	std::atomic<Status> status{Status::Unlock};

	Status cas(Status expected, Status desire) {
		status.compare_exchange_weak(expected, desire);
		return expected;
	}

	bool unlock_has_contention() {
		return status.exchange(Unlock) == LockContention;
	}

	uint32_t* cast() {
		return reinterpret_cast<uint32_t*>(&status);
	}

	void park() {
		if (status.load() == LockContention || cas(Lock, LockContention) == Lock) {
			impl::wait(cast(), LockContention);
		}
	}
};

} // namespace sync_cpp
