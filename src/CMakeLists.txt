cmake_minimum_required(VERSION 3.25)

include_directories(.)
add_library(Sync
        common/sys_waiter.cpp
        mutex/mutex.cpp
        cond_var/cond_var.cpp
)

target_include_directories(Sync PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(Sync PROPERTIES LINKER_LANGUAGE CXX)