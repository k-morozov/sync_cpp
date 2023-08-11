//
// Created by focus on 4/2/23.
//

#include <benchmark/benchmark.h>

#include <thread>
#include <vector>

#include "spinlock.h"

static constexpr size_t CountThreads = 8;
static constexpr size_t CountIncrement = 100'000;


static void simple_spinlock(benchmark::State& state) {
	for (auto _ : state) {
		std::vector<std::jthread> workers;
		uint64_t counter = UINT64_MAX;
		sync_cpp::Spinlock<sync_cpp::SimplePolicy> spinlock;

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
}
BENCHMARK(simple_spinlock)->Unit(benchmark::kMillisecond)->Iterations(10);

static void readable_spinlock(benchmark::State& state) {
	for (auto _ : state) {
		std::vector<std::jthread> workers;
		uint64_t counter = UINT64_MAX;
		sync_cpp::Spinlock<sync_cpp::ReadablePolicy> spinlock;

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
}
BENCHMARK(readable_spinlock)->Unit(benchmark::kMillisecond)->Iterations(10);

static void mm_readable_spinlock(benchmark::State& state) {
	for (auto _ : state) {
		std::vector<std::jthread> workers;
		uint64_t counter = UINT64_MAX;
		sync_cpp::Spinlock<sync_cpp::MMReadablePolicy> spinlock;

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
}
BENCHMARK(mm_readable_spinlock)->Unit(benchmark::kMillisecond)->Iterations(10);


//BENCHMARK_MAIN();