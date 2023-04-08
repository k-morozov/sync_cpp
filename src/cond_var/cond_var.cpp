//
// Created by focus on 4/8/23.
//

#include "cond_var.h"



namespace sync_cpp {

void CondVar::notify_one() {
	counter_.fetch_add(1);
	sys_waiter::futex_wake(cast_counter(), 1);
}

} // namespace sync_cpp