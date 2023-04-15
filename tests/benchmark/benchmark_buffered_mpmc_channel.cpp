//
// Created by focus on 4/2/23.
//

#include <benchmark/benchmark.h>

#include "common/buffered_mpmc_executor.h"

#define BUFFERED_MPMC_CHANNEL_BENCHMARK(N1, N2, N3) \
    static void BufferedChannel_MPMC_##N1##_##N2##_##N3(benchmark::State& state) { \
        for (auto _ : state) { \
            BufferedMPMCExecutor<int> ex(N1, N2, N3); \
            ex(); \
        } \
    } \
    BENCHMARK(BufferedChannel_MPMC_##N1##_##N2##_##N3)->Unit(benchmark::kMillisecond)

BUFFERED_MPMC_CHANNEL_BENCHMARK(2, 2, 2);
BUFFERED_MPMC_CHANNEL_BENCHMARK(32, 64, 16);
BUFFERED_MPMC_CHANNEL_BENCHMARK(64, 32, 16);
BUFFERED_MPMC_CHANNEL_BENCHMARK(32, 32, 16);


//BENCHMARK_MAIN();