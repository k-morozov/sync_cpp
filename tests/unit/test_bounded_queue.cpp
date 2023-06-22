//
// Created by focus on 4/3/23.
//

#include "gtest/gtest.h"

#include <thread>
#include <vector>
#include <atomic>
//#include <ranges>
#include <algorithm>

#include <common/bounded_queue.h>
#include <queue/mpmc.h>
#include <common/common.h>

using namespace std::chrono_literals;

constexpr bool log = false;

class BoundedQueueTest : public ::testing::Test {
public:
	using Queue = BoundedQueue<int>;
};

TEST_F(BoundedQueueTest, simple_enqueue) {
	Queue queue{2};
	ASSERT_TRUE(queue.Enqueue(2));
	ASSERT_TRUE(queue.Enqueue(2));
	ASSERT_FALSE(queue.Enqueue(2));
	ASSERT_FALSE(queue.Enqueue(2));
}

TEST_F(BoundedQueueTest, simple_dequeue) {
	int val;
	Queue queue{2};
	ASSERT_FALSE(queue.Dequeue(val));
	ASSERT_FALSE(queue.Dequeue(val));
}

TEST_F(BoundedQueueTest, simple_enqueue_dequeue) {
	auto val = 0;
	Queue queue{2};
	ASSERT_TRUE(queue.Enqueue(1));
	ASSERT_TRUE(queue.Dequeue(val));
	ASSERT_TRUE(val == 1);
	ASSERT_FALSE(queue.Dequeue(val));

	ASSERT_TRUE(queue.Enqueue(2));
	ASSERT_TRUE(queue.Enqueue(3));
	ASSERT_FALSE(queue.Enqueue(4));

	ASSERT_TRUE(queue.Dequeue(val));
	ASSERT_TRUE(val == 2);
	ASSERT_TRUE(queue.Dequeue(val));
	ASSERT_TRUE(val == 3);

	ASSERT_FALSE(queue.Dequeue(val));
}

TEST_F(BoundedQueueTest, wo_spirious_fails) {
//	static constexpr auto kRange = std::views::iota(0, 128 * 1024);
	std::vector<int> kRange;
	for(int i=0; i<128*1024; i++) {
		kRange.push_back(i);
	}
	static constexpr auto kNumThreads = 8;
	const auto kN = kNumThreads * kRange.size();
	Queue queue{kN};
	std::atomic counter = 0;

	std::vector<std::jthread> threads;
	for (auto i = 0; i < kNumThreads; ++i) {
		threads.emplace_back([&] {
			for (auto x : kRange) {
				counter += queue.Enqueue(x);
			}
		});
	}
	threads.clear();
	ASSERT_TRUE(counter == kN);

	for (auto i = 0; i < kNumThreads; ++i) {
		threads.emplace_back([&] {
			for (auto x : kRange) {
				counter -= queue.Dequeue(x);
			}
		});
	}
	threads.clear();
	ASSERT_TRUE(counter == 0);
}

TEST_F(BoundedQueueTest, no_queue_lock) {
////	static constexpr auto kRange = std::views::iota(0, 100'000);
//
//	std::vector<int> kRange;
//	for(int i=0; i<100'000; i++) {
//		kRange.push_back(i);
//	}
//	static constexpr auto kNumThreads = 8;
//	const auto kN = kNumThreads * kRange.size();
//
//	std::vector<std::atomic<int>> results;
//	static constexpr auto kNumProducers = 4;
//	static constexpr auto kNumConsumers = 4;
//	Queue queue{64};
//
//	std::vector<std::jthread> threads;
//	for (auto i = 0; i < kNumProducers; ++i) {
//		threads.emplace_back([&] {
//			for (auto x : kRange) {
//				if (queue.Enqueue(x)) {
//					++results[x];
//				}
//			}
//		});
//	}
//	for (auto i = 0; i < kNumConsumers; ++i) {
//		threads.emplace_back([&] {
//			for (auto x : kRange) {
//				if (queue.Dequeue(x)) {
//					--results[x];
//				}
//			}
//		});
//	}
//	threads.clear();
//
//	auto k = 0;
//	while (queue.Dequeue(k)) {
//		--results[k];
//	}
//	for(const auto & r : results) {
//		ASSERT_EQ(r, 0);
//	}
////	ASSERT_TRUE(std::ranges::all_of(results, [](const auto& a) { return a == 0; }));
//	ASSERT_TRUE(queue.Enqueue(0));
//	ASSERT_TRUE(queue.Dequeue(k));
//	ASSERT_TRUE(k == 0);
}

TEST_F(BoundedQueueTest, stress_enqueue) {
	for (auto num_threads : {1, 2, 4, 8}) {
		Queue queue{64};
		StressEnqueue(queue, num_threads, log);
	}
}

TEST_F(BoundedQueueTest, stress_enqueue_dequeue) {
	for (auto num_threads : {1, 2, 4}) {
		Queue queue{64};
		StressEnqueueDequeue(queue, num_threads, num_threads, log);
	}
}

TEST_F(BoundedQueueTest, correctness_enqueue_dequeue) {
	for (auto num_threads : {1, 2, 4}) {
		Queue queue{64};
		CorrectnessEnqueueDequeue(queue, num_threads, num_threads, log);
	}
}

