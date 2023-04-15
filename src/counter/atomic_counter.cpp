//
// Created by focus on 4/9/23.
//
#include "atomic_counter.h"

namespace sync_cpp {


void AtomicCounter::Increment() {
	value_.fetch_add(1);
}

size_t AtomicCounter::Get() const {
	return value_.load();
}
} // namespace sync_cpp