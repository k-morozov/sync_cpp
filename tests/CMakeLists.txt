cmake_minimum_required(VERSION 3.25)

include_directories(${CONAN_INCLUDE_DIRS_GTEST})
include_directories(${CONAN_INCLUDE_DIRS_BENCHMARK})

set(TEST_APP_NAME unit_test_sync)
#set(BENCHMARK_APP_NAME benchmark_sync)

add_executable(${TEST_APP_NAME}
        unit/main.cpp
        unit/test_mutex.cpp
)

target_link_libraries(${TEST_APP_NAME}
        Sync
        ${CONAN_LIBS_GTEST}
)

#add_executable(${BENCHMARK_APP_NAME}
#        benchmark/main.cpp
#        benchmark/benchmark_buffered_channel.cpp
#        benchmark/benchmark_unbuffered_channel.cpp
#        benchmark/benchmark_mpmc_bounded_queue_int.cpp
#        benchmark/benchmark_mpmc_bounded_queue_data.cpp
#        benchmark/benchmark_buffered_mpmc_channel.cpp
#)
#
#target_link_libraries(${BENCHMARK_APP_NAME}
#        Channels
#        ${CONAN_LIBS_GTEST}
#        ${CONAN_LIBS_BENCHMARK}
#        )

enable_testing()