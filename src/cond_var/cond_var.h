//
// Created by focus on 4/8/23.
//

#pragma once

#include <atomic>
#include <memory>

#include <common/sys_waiter.h>

namespace sync_cpp {

class CondVar final {
public:
	template<typename Lockable>
	void wait(Lockable & l) {
		const auto old_index = counter_.load();
		l.unlock();
		sys_waiter::futex_wait(cast_counter(), old_index);
		l.lock();
	}

	void notify_one();

private:
	std::atomic<uint32_t> counter_{0};

	uint32_t * cast_counter() {
		return reinterpret_cast<uint32_t *>(&counter_);
	}
};

} // namespace sync_cpp
