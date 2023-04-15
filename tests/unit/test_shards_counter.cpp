//
// Created by focus on 4/3/23.
//

#include "gtest/gtest.h"

#include <thread>

#include "counter/shards_counter.h"

#include <common/runner.h>

class TestShardsCounter : public ::testing::Test {
public:
};

TEST_F(TestShardsCounter, simple_counter) {
	sync_cpp::ShardsCounter<8> counter;

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

