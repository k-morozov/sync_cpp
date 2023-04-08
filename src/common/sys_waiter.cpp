//
// Created by focus on 4/8/23.
//

#include "sys_waiter.h"

#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

namespace sync_cpp::sys_waiter {

void futex_wait(uint32_t * state, const uint32_t expected_value) {
	syscall(SYS_futex, state, FUTEX_WAIT, expected_value, nullptr, nullptr, 0);
}

void futex_wake(uint32_t * state, const int count_waiters_to_wake_up) {
	syscall(SYS_futex, state, FUTEX_WAKE, count_waiters_to_wake_up, nullptr, nullptr, 0);
}


} // namespace sync_cpp::sys_waiter