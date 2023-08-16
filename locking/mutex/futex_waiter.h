//
// Created by focus on 8/16/23.
//

#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <cstdint>
#include <unistd.h>

namespace sync_cpp::impl {

void wait(uint32_t* state, const uint32_t current) {
	syscall(SYS_futex, state, FUTEX_WAIT, current, nullptr, nullptr, 0);
}

void wake(uint32_t* state) {
	syscall(SYS_futex, state, FUTEX_WAKE, 1, nullptr, nullptr, 0);
}

} // namespace sync_cpp::impl
