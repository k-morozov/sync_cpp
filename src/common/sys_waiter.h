//
// Created by focus on 4/8/23.
//

#pragma once

#include <cstdint>

namespace sync_cpp::sys_waiter {

void futex_wait(uint32_t * state, uint32_t expected_value);

void futex_wake(uint32_t * state, int count_waiters_to_wake_up);

} // namespace sync_cpp::sys_waiter