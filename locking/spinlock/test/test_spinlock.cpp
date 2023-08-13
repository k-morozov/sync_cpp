//
// Created by focus on 6/20/23.
//

#include "gtest/gtest.h"

#include <thread>

#include <spinlock.h>

static constexpr size_t CountThreads = 16;
static constexpr size_t CountIncrement = 100'000;

class TestSpinlock : public ::testing::Test {
public:
};


template<class T>
void run_test_spinlock() {
	uint64_t counter = 0;
	{
		std::vector<std::jthread> workers;
		sync_cpp::Spinlock<T> spinlock;

		for(size_t i=0; i<CountThreads; i++) {
			workers.emplace_back([&counter, &spinlock] {
				for (size_t i=0; i<CountIncrement; i++) {
					spinlock.lock();
					counter++;
					spinlock.unlock();
				}
			});
		}
	}

	ASSERT_EQ(counter, CountThreads * CountIncrement);
}

template<typename T, typename ...Args>
void run_tests_spinlock() {
	run_test_spinlock<T>();
	if constexpr (sizeof...(Args) > 0) {
		run_tests_spinlock<Args...>();
	}
}

TEST_F(TestSpinlock, simple) {
	run_tests_spinlock<
	        sync_cpp::SimplePolicy,
			sync_cpp::ReadablePolicy,
			sync_cpp::MMReadablePolicy>();
}
