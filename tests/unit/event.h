//
// Created by focus on 4/8/23.
//

#pragma once

#include <mutex>

#include <mutex/mutex.h>
#include <cond_var/cond_var.h>

namespace test_space {

class Event final {
public:
	void done() {
		std::lock_guard lock(m_);
		event_ = true;
		has_event_.notify_one();
	}

	void wait() {
		std::unique_lock lock(m_);
		while (!event_) {
			has_event_.wait(lock);
		}
	}

private:
	sync_cpp::Mutex m_;
	sync_cpp::CondVar has_event_;

	bool event_{};
};


} // namespace test_space