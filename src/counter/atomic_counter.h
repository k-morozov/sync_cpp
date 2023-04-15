//
// Created by focus on 4/9/23.
//

#pragma once

#include <atomic>

namespace sync_cpp {

class AtomicCounter final {
public:
	void Increment();
	size_t Get() const;

private:
	std::atomic<size_t> value_{};
};

} // namespace sync_cpp