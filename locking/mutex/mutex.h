//
// Created by focus on 8/11/23.
//

#pragma once

#include <atomic>

namespace sync_cpp {

class SimpleMutex final {
	enum Status : uint32_t {
		Lock,
		Unlock,
		Wait
	};

public:
	void lock() {
		Status old = status.load();
		if (status.compare_exchange_weak(old, Status::Lock)) {
			return;
		}


	}

	void unlock() {}

private:
	std::atomic<Status> status{Status::Unlock};
};

} // namespace sync_cpp
