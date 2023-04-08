//
// Created by focus on 4/2/23.
//

#include <benchmark/benchmark.h>

#include "common/unbuffered_executor.h"

#define UNBUFFERED_CHANNEL_BENCHMARK(N1, N2) \
    static void UnbufferedChannel_##N1##_##N2(benchmark::State& state) { \
        for (auto _ : state) { \
            UnbufferedExecutor<int> ex(N1, N2); \
            ex(); \
        } \
    } \
    BENCHMARK(UnbufferedChannel_##N1##_##N2)->Unit(benchmark::kMillisecond)

UNBUFFERED_CHANNEL_BENCHMARK(1, 1);
UNBUFFERED_CHANNEL_BENCHMARK(32, 64);
UNBUFFERED_CHANNEL_BENCHMARK(64, 32);
UNBUFFERED_CHANNEL_BENCHMARK(32, 32);


//BENCHMARK_MAIN();