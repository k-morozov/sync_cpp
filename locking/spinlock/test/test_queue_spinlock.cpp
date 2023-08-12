//
// Created by focus on 6/20/23.
//

#include "gtest/gtest.h"

#include <thread>

#include <queue_spinlock.h>

static constexpr size_t CountThreads = 8;
static constexpr size_t CountIncrement = 100'000;

class TestQueueSpinlock : public ::testing::Test {
public:
};

using Guard = sync_cpp::QueueSpinlock::Guard;

void run_test_queue_spinlock() {
	uint64_t counter = 0;
	{
		std::vector<std::jthread> workers;
		sync_cpp::QueueSpinlock spinlock;

		for(size_t i=0; i<CountThreads; i++) {
			workers.emplace_back([&counter, &spinlock] {
				for (size_t i=0; i<CountIncrement; i++) {
					Guard g(spinlock);
					counter++;
				}
			});
		}
	}

	ASSERT_EQ(counter, CountThreads * CountIncrement);
}

TEST_F(TestQueueSpinlock, simple) {
	run_test_queue_spinlock();
}
