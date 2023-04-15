//
// Created by focus on 4/2/23.
//

#include <benchmark/benchmark.h>

#include "common/buffered_executor.h"

#define BUFFERED_CHANNEL_BENCHMARK(N1, N2, N3) \
    static void BufferedChannel_##N1##_##N2##_##N3(benchmark::State& state) { \
        for (auto _ : state) { \
            BufferedExecutor<int> ex(N1, N2, N3); \
            ex(); \
        } \
    } \
    BENCHMARK(BufferedChannel_##N1##_##N2##_##N3)->Unit(benchmark::kMillisecond)

BUFFERED_CHANNEL_BENCHMARK(1, 1, 1);
BUFFERED_CHANNEL_BENCHMARK(2, 2, 2);
BUFFERED_CHANNEL_BENCHMARK(32, 64, 16);
BUFFERED_CHANNEL_BENCHMARK(64, 32, 16);
BUFFERED_CHANNEL_BENCHMARK(32, 32, 16);


//BENCHMARK_MAIN();