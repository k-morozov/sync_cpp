cmake_minimum_required(VERSION 3.25)

include_directories(.)
add_library(queue_spsc
        queue.h
)

target_link_libraries(queue_spsc
        atomic
        project_sanitizers
        project_warnings
)

target_include_directories(queue_spsc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(queue_spsc PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(test)