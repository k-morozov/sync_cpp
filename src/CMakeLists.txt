cmake_minimum_required(VERSION 3.25)

include_directories(.)
add_library(Sync
        mutex/mutex.h
)

set_target_properties(Sync PROPERTIES LINKER_LANGUAGE CXX)