//
// Created by focus on 4/3/23.
//

#include "gtest/gtest.h"

#include <thread>

#include "counter/atomic_counter.h"

#include <common/runner.h>

using namespace std::chrono_literals;

class TestSimpleCounter : public ::testing::Test {
public:
};

TEST_F(TestSimpleCounter, simple_counter) {
	sync_cpp::AtomicCounter counter;

	{
		std::jthread th1([&counter] {
			test_space::run(counter, 1000);
		});
		std::jthread th2([&counter] {
			test_space::run(counter, 1000);
		});
		std::jthread th3([&counter] {
			test_space::run(counter, 1000);
		});
	}

	ASSERT_EQ(counter.Get(), 3000u);
}

