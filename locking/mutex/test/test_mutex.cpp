//
// Created by focus on 6/20/23.
//

#include "gtest/gtest.h"

#include <thread>

#include <mutex.h>

static constexpr size_t CountThreads = 16;
static constexpr size_t CountIncrement = 100'000;

class TestMutex : public ::testing::Test {
public:
};


void run_test_mutex() {
	uint64_t counter = 0;
	{
		std::vector<std::jthread> workers;
		sync_cpp::SimpleMutex m;

		for(size_t i=0; i<CountThreads; i++) {
			workers.emplace_back([&counter, &m] {
				for (size_t i=0; i<CountIncrement; i++) {
					m.lock();
					counter++;
					m.unlock();
				}
			});
		}
	}

	ASSERT_EQ(counter, CountThreads * CountIncrement);
}


TEST_F(TestMutex, simple) {
	run_test_mutex();
}
