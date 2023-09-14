//
// Created by focus on 9/12/23.
//

#pragma once

#include <atomic>
#include <exception>
#include <memory>
#include <thread>

namespace sync_cpp {

static constexpr size_t MaxHazardPointers = 100;

struct HazardPtr {
	std::atomic<std::thread::id> id;
	std::atomic<void *> ptr;
};

HazardPtr hazard_ptrs[MaxHazardPointers];

template<typename T>
void do_delete(void *p) {
	delete static_cast<T *>(p);
}

} // namespace sync_cpp