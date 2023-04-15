//
// Created by focus on 4/3/23.
//
#include <benchmark/benchmark.h>

#include <random>

#include <common/common.h>
#include <common/bounded_queue.h>
#include <queue/mpmc.h>

namespace detail {

template<typename T>
T generate() {
	throw std::runtime_error("broken spec");
}

template<>
int generate<int>() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 100);

	return dis(gen);
}

template<class T>
struct Data {
	T value;
	size_t pos;
};

template<template <typename> class Q, typename T>
void Run(Q<Data<T>> & queue, const uint32_t num_producers, const uint32_t num_consumers) {
	std::vector<std::atomic<T>> enqueued(num_producers);
	std::vector<std::atomic<T>> dequeued(num_producers);
	static constexpr size_t Count = 100'000;
	test_space::Runner prod_runner{Count};
	for (auto i = 0u; i < num_producers; ++i) {
		auto done = std::make_shared<int>(0);
		auto func = [&, i, done] {
			*done += queue.Enqueue(Data<T>{
				.value=generate<T>(),
				.pos=i
			});
		};
		auto on_exit = [&, i, done] { enqueued[i] = *done; };
		prod_runner.Do(test_space::TaskWithExit{std::move(func), std::move(on_exit)});
	}
	test_space::Runner cons_runner{Count};
	for (auto i = 0u; i < num_consumers; ++i) {
		auto counters = std::make_shared<std::vector<int>>(num_producers);
		auto func = [&, counters, &c = *counters] {
			if (Data<T> value{}; queue.Dequeue(value)) {
				++c[value.pos];
			}
		};
		auto on_exit = [&, counters, &c = *counters] {
			for (auto i = 0u; i < num_producers; ++i) {
				dequeued[i] += c[i];
			}
		};
		cons_runner.Do(test_space::TaskWithExit{std::move(func), std::move(on_exit)});
	}

	prod_runner.Wait();
	cons_runner.Wait();

	Data<T> value{};
	while (queue.Dequeue(value)) {
		++dequeued[value.pos];
	}
	ASSERT_TRUE(std::ranges::equal(enqueued, dequeued));
}

}

#define RUN_DATA_QUEUES(Type, N1, N2, N3) \
    static void DefaultBoundedQueue_Data_##N1##_##N2##_##N3(benchmark::State& state) { \
        BoundedQueue<detail::Data<Type>> queue{N3}; \
        for (auto _ : state) { \
            detail::Run<BoundedQueue, Type>(queue, N1, N2); \
        } \
    } \
    BENCHMARK(DefaultBoundedQueue_Data_##N1##_##N2##_##N3)->Unit(benchmark::kMillisecond)->Iterations(10); \
    static void MPMCQueue_Data_##N1##_##N2##_##N3(benchmark::State& state) { \
        sync_cpp::queue::MPMCUnboundedQueue<detail::Data<Type>> queue{N3}; \
        for (auto _ : state) { \
            detail::Run<sync_cpp::queue::MPMCUnboundedQueue, Type>(queue, N1, N2); \
        } \
    } \
    BENCHMARK(MPMCQueue_Data_##N1##_##N2##_##N3)->Unit(benchmark::kMillisecond)->Iterations(10);


RUN_DATA_QUEUES(int, 16, 32, 32);
RUN_DATA_QUEUES(int, 32, 16, 32);
RUN_DATA_QUEUES(int, 32, 32, 16);
RUN_DATA_QUEUES(int, 32, 32, 64);

