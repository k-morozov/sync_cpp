cmake_minimum_required(VERSION 3.25)

set(BENCHMARK_APP_NAME benchmark_mutex)

include_directories(.)
add_library(mutex
        mutex.h
        futex_waiter.h
)

target_link_libraries(spinlock
        atomic
        project_sanitizers
        project_warnings
)

target_include_directories(mutex PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CONAN_INCLUDE_DIRS_GTEST})
include_directories(${CONAN_INCLUDE_DIRS_BENCHMARK})

set_target_properties(mutex PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(test)

#add_executable(${BENCHMARK_APP_NAME}
#        bench/main.cpp
#        bench/bench.cpp
#)

#target_link_libraries(${BENCHMARK_APP_NAME}
#        Sync
#        ${CONAN_LIBS_GTEST}
#        ${CONAN_LIBS_BENCHMARK}
#        )