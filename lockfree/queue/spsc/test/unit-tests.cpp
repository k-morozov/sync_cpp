//
// Created by focus on 9/13/23.
//

#include "gtest/gtest.h"

#include <thread>

#include <queue.h>

class TestSPSCQueue : public ::testing::Test {
public:
};

TEST_F(TestSPSCQueue, push_pop) {
	sync_cpp::SPSCQueue<int> q;

	q.push(117);
	auto item = q.pop();
	ASSERT_TRUE(item);
	ASSERT_EQ(*item, 117);

	auto empty = q.pop();
	ASSERT_FALSE(empty);
}

TEST_F(TestSPSCQueue, only_pop) {
	sync_cpp::SPSCQueue<std::string> stack;

	auto empty = stack.pop();
	ASSERT_FALSE(empty);

	empty = stack.pop();
	ASSERT_FALSE(empty);
}

TEST_F(TestSPSCQueue, 2_threads) {
	sync_cpp::SPSCQueue<size_t> q;

	auto th1 = std::jthread([&] {
		for (size_t i=0; i<20'000; i++) {
			q.push(i);
		}
	});

	std::vector<size_t> result;
	result.reserve(20'000);
	size_t counter = 0;
	while(true) {
		auto data = q.pop();
		if (!data) {
			continue;
		}
		counter++;
		result.push_back(*data);
		if (counter == 20'000) {
			break;
		}
	}

	std::sort(result.begin(), result.end());

	for(size_t i=0; i<20'000; i++) {
		ASSERT_EQ(result[i], i);
	}
}