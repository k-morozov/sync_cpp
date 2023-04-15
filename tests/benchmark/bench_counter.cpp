//
// Created by focus on 4/2/23.
//

#include <benchmark/benchmark.h>

#include <thread>
#include <vector>

#include "counter/atomic_counter.h"
#include "counter/shards_counter.h"
#include <common/runner.h>

static constexpr size_t CountThreads = 16;
static constexpr size_t CountIncrement = 100'000;


static void simple_counter(benchmark::State& state) {
	for (auto _ : state) {
		std::vector<std::jthread> workers;
		sync_cpp::AtomicCounter counter;

		for(size_t i=0; i<CountThreads; i++) {
			workers.emplace_back([&counter] {
				test_space::run(counter, CountIncrement);
			});
		}
	}
}
BENCHMARK(simple_counter)->Unit(benchmark::kMicrosecond)->Iterations(10);

static void shards_counter(benchmark::State& state) {
	for (auto _ : state) {
		std::vector<std::jthread> workers;
		sync_cpp::ShardsCounter<CountThreads> counter;

		for(size_t i=0; i<CountThreads; i++) {
			workers.emplace_back([&counter] {
				test_space::run(counter, CountIncrement);
			});
		}
	}
}
BENCHMARK(shards_counter)->Unit(benchmark::kMicrosecond)->Iterations(10);


//BENCHMARK_MAIN();