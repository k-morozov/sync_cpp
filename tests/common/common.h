//
// Created by focus on 4/1/23.
//

#pragma once

#include "gtest/gtest.h"

#include <algorithm>
#include <ranges>
#include <thread>

#include <channel.h>

#include <common/bounded_queue.h>
#include <common/runner.h>

using namespace std::chrono_literals;

void CheckClose(auto& chan) {
	using input_channel_type = typename std::decay_t<decltype(chan)>::input_channel_type;
	std::thread th([](input_channel_type & chan){
			ASSERT_THROW(chan << 1, std::runtime_error);
	}, std::ref(chan));
	if (th.joinable()) {
		th.join();
	}
	std::optional<int> value;
	chan >> value;
	ASSERT_TRUE(value == std::nullopt);
}

void CheckDataRace(auto& chan) {
	using input_channel_type = typename std::decay_t<decltype(chan)>::input_channel_type;
	using output_channel_type = typename std::decay_t<decltype(chan)>::output_channel_type;

	for (int i=0; i<1'000; i++) {
		int number = 100500 + i;

		std::thread th1([&number, i](input_channel_type & chan) {
			number = 99 + i;
			chan << 11 + i;
		}, std::ref(chan));

		std::thread th2([&number, i](output_channel_type & chan) {
			std::optional<int> n;
			chan >> n;

			ASSERT_TRUE(n && n.value() == 11 + i);
			ASSERT_EQ(number, 99 + i);
		}, std::ref(chan));

		if (th1.joinable()) {
			th1.join();
		}
		if (th2.joinable()) {
			th2.join();
		}
	}
}

void CheckCopy(auto& channel) {
	using input_channel_type = typename std::decay_t<decltype(channel)>::input_channel_type;

	std::vector v{1,2,3,4,5};
	std::thread th([v](input_channel_type & chan){
		chan << v;
	}, std::ref(channel));

	std::optional<decltype(v)> result;
	channel >> result;
	const auto expected = v;
	v.clear();

	if (th.joinable()) {
		th.join();
	}
	ASSERT_TRUE(result);
	ASSERT_TRUE(result.value() == expected);
}

template<typename Q>
void StressEnqueue(Q & queue, uint32_t num_producers, bool log = true) {
	test_space::TimeRunner runner{1s};
	for (auto i = 0u; i < num_producers; ++i) {
		runner.Do([&] { queue.Enqueue(0); });
	}
	const auto run_wait = runner.Wait();
	if (log) {
		GTEST_LOG_(INFO) << std::to_string(num_producers);
		GTEST_LOG_(INFO) << "Runner wait: " <<  run_wait << ", limit=" << 100ns;
	}
}

template<typename Q>
void StressEnqueueDequeue(Q & queue, uint32_t num_producers, uint32_t num_consumers, bool log = true) {
	test_space::TimeRunner prod_runner{1s};
	for (auto i = 0u; i < num_producers; ++i) {
		prod_runner.Do([&] { queue.Enqueue(0); });
	}
	test_space::TimeRunner cons_runner{1s};
	for (auto i = 0u; i < num_consumers; ++i) {
		cons_runner.Do([&, x = 0]() mutable { queue.Dequeue(x); });
	}

	const auto prod_wait = prod_runner.Wait();
	const auto cons_wait = cons_runner.Wait();
	if (log) {
		GTEST_LOG_(INFO) << std::to_string(num_producers) << " " << std::to_string(num_consumers);
		GTEST_LOG_(INFO) << "Produce runner wait: " <<  prod_wait << ", limit=" << 100ns;
		GTEST_LOG_(INFO) << "Consumer runner wait: " <<  cons_wait << ", limit=" << 100ns;
	}
}

template<typename Q>
void CorrectnessEnqueueDequeue(Q & queue, uint32_t num_producers, uint32_t num_consumers, bool log = true) {
	std::vector<std::atomic<size_t>> enqueued(num_producers);
	std::vector<std::atomic<size_t>> dequeued(num_producers);
	test_space::TimeRunner prod_runner{1s};
	for (auto i = 0u; i < num_producers; ++i) {
		auto c = std::make_shared<size_t>(0);
		auto func = [&, i, c] { *c += queue.Enqueue(i); };
		auto on_exit = [&, i, c] { enqueued[i] = *c; };
		prod_runner.Do(test_space::TaskWithExit{std::move(func), std::move(on_exit)});
	}
	test_space::TimeRunner cons_runner{1s};
	for (auto i = 0u; i < num_consumers; ++i) {
		auto counters = std::make_shared<std::vector<size_t>>(num_producers);
		auto func = [&, counters, &c = *counters] {
			if (int value; queue.Dequeue(value)) {
				++c[static_cast<unsigned long>(value)];
			}
		};
		auto on_exit = [&, counters, &c = *counters] {
			for (auto i = 0u; i < num_producers; ++i) {
				dequeued[i] += static_cast<unsigned long>(c[i]);
			}
		};
		cons_runner.Do(test_space::TaskWithExit{std::move(func), std::move(on_exit)});
	}

	const auto prod_wait = prod_runner.Wait();
	const auto cons_wait = cons_runner.Wait();
	if (log) {
		GTEST_LOG_(INFO) << std::to_string(num_producers) << " " << std::to_string(num_consumers);
		GTEST_LOG_(INFO) << "Produce runner wait: " <<  prod_wait << ", limit=" << 100ns;
		GTEST_LOG_(INFO) << "Consumer runner wait: " <<  cons_wait << ", limit=" << 100ns;
	}

	int value;
	while (queue.Dequeue(value)) {
		++dequeued[value];
	}
	ASSERT_TRUE(std::ranges::equal(enqueued, dequeued));
}