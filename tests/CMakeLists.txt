cmake_minimum_required(VERSION 3.25)

include_directories(${CONAN_INCLUDE_DIRS_GTEST})
include_directories(${CONAN_INCLUDE_DIRS_BENCHMARK})

set(TEST_APP_NAME unit_test_sync)
set(BENCHMARK_APP_NAME benchmark_sync)

add_executable(${TEST_APP_NAME}
        unit/main.cpp

        common/runner.h
        common/common.h
        common/buffered_executor.h
        common/buffered_mpmc_executor.h
        unit/test_buffered_channel.cpp
        unit/test_unbuffered_channel.cpp
        unit/test_bounded_queue.cpp
        unit/test_mpmc_unbounded_queue.cpp
        unit/test_buffered_mpmc_channel.cpp
        unit/test_mutex.cpp
        unit/test_counter.cpp
        unit/test_shards_counter.cpp
)

target_link_libraries(${TEST_APP_NAME}
        Sync
        ${CONAN_LIBS_GTEST}
)

add_executable(${BENCHMARK_APP_NAME}
        benchmark/main.cpp
        benchmark/bench_counter.cpp
        benchmark/benchmark_buffered_channel.cpp
        benchmark/benchmark_unbuffered_channel.cpp
        benchmark/benchmark_mpmc_bounded_queue_int.cpp
        benchmark/benchmark_mpmc_bounded_queue_data.cpp
        benchmark/benchmark_buffered_mpmc_channel.cpp
)

target_link_libraries(${BENCHMARK_APP_NAME}
        Sync
        ${CONAN_LIBS_GTEST}
        ${CONAN_LIBS_BENCHMARK}
        )

enable_testing()