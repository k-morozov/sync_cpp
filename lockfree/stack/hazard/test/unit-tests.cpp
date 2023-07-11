//
// Created by focus on 7/11/23.
//

#include "gtest/gtest.h"

#include <thread>

#include <stack.h>

class TestLockFreeStackHazard : public ::testing::Test {
public:
};

TEST_F(TestLockFreeStackHazard, push_pop) {
	sync_cpp::LockFreeStackHazard<int> stack;

	stack.Push(117);
	auto item = stack.Pop();
	ASSERT_TRUE(item);
	ASSERT_EQ(*item, 117);

	auto empty = stack.Pop();
	ASSERT_FALSE(empty);
}

TEST_F(TestLockFreeStackHazard, only_pop) {
	sync_cpp::LockFreeStackHazard<std::string> stack;

	auto empty = stack.Pop();
	ASSERT_FALSE(empty);

	empty = stack.Pop();
	ASSERT_FALSE(empty);
}

TEST_F(TestLockFreeStackHazard, push_some_pop_push) {
	sync_cpp::LockFreeStackHazard<std::string> stack;

	stack.Push("Data");
	auto item = stack.Pop();
	ASSERT_TRUE(item);
	ASSERT_EQ(*item, "Data");

	auto empty = stack.Pop();
	ASSERT_FALSE(empty);

	empty = stack.Pop();
	ASSERT_FALSE(empty);

	stack.Push("Data");
	item = stack.Pop();
	ASSERT_TRUE(item);
	ASSERT_EQ(*item, "Data");

	empty = stack.Pop();
	ASSERT_FALSE(empty);
}

TEST_F(TestLockFreeStackHazard, LIFO) {
	sync_cpp::LockFreeStackHazard<int> stack;

	stack.Push(1);
	stack.Push(2);
	stack.Push(3);

	ASSERT_EQ(*stack.Pop(), 3);
	ASSERT_EQ(*stack.Pop(), 2);
	ASSERT_EQ(*stack.Pop(), 1);

	ASSERT_FALSE(stack.Pop());
}

TEST_F(TestLockFreeStackHazard, Dtor) {
	sync_cpp::LockFreeStackHazard<std::string> stack;

	stack.Push("Hello");
	stack.Push("World");
}

TEST_F(TestLockFreeStackHazard, 2_stacks) {
	sync_cpp::LockFreeStackHazard<int> stack_1;
	sync_cpp::LockFreeStackHazard<int> stack_2;

	stack_1.Push(3);
	stack_2.Push(11);
	ASSERT_EQ(*stack_1.Pop(), 3);
	ASSERT_EQ(*stack_2.Pop(), 11);
}

TEST_F(TestLockFreeStackHazard, 2_threads_push) {
	sync_cpp::LockFreeStackHazard<size_t> stack;

	auto th1 = std::jthread([&] {
		for (size_t i=0; i<10'000; i++) {
			stack.Push(i);
		}
	});
	auto th2 = std::jthread([&] {
		for (size_t i=10'000; i<20'000; i++) {
			stack.Push(i);
		}
	});

	std::vector<size_t> result;
	result.reserve(20'000);
	size_t counter = 0;
	while(true) {
		auto data = stack.Pop();
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

TEST_F(TestLockFreeStackHazard, 2_threads_pop) {
	sync_cpp::LockFreeStackHazard<size_t> stack;

	auto th1 = std::thread([&] {
		for (size_t i=0; i<20'000; i++) {
			stack.Push(i);
		}
	});

	std::atomic<size_t> counter = 0;

	std::vector<size_t> result_first_internal;
	std::vector<size_t> result_second_internal;

	auto th1_pop = std::jthread([&] {
		while(counter < 20'000) {
			auto data = stack.Pop();
			if (!data) {
				continue;
			}
			counter++;
			result_first_internal.push_back(*data);
		}
		std::sort(result_first_internal.begin(), result_first_internal.end());
	});

	auto th2_pop = std::jthread([&] {
		while(counter < 20'000) {
			auto data = stack.Pop();
			if (!data) {
				continue;
			}
			counter++;
			result_second_internal.push_back(*data);
		}
		std::sort(result_second_internal.begin(), result_second_internal.end());
	});

	th1.join();
	th1_pop.join();
	th2_pop.join();

	std::vector<size_t> result;
	result.reserve(20'000);

	std::merge(result_first_internal.begin(), result_first_internal.end(),
			   result_second_internal.begin(), result_second_internal.end(),
			   std::back_inserter(result));
	std::sort(result.begin(), result.end());

	for(size_t i=0; i<20'000; i++) {
		ASSERT_EQ(result[i], i);
	}
}