cmake_minimum_required(VERSION 3.25)

include_directories(.)
add_library(Sync
        cond_var/cond_var.cpp
        counter/atomic_counter.cpp
        counter/shards_counter.h
        channel.h
        detail/channel_base.h
        policy/buffered_policy.h
        policy/unbuffered_policy.h
        policy/buffered_mpmc_policy.h
        queue/mpmc.h
)

target_link_libraries(Sync
        atomic
        project_sanitizers
        project_warnings
)

target_include_directories(Sync PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(Sync PROPERTIES LINKER_LANGUAGE CXX)