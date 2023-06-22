cmake_minimum_required(VERSION 3.25)

include_directories(.)
add_library(stack_counting
        stack.h
)

target_link_libraries(stack_counting
        atomic
        project_sanitizers
        project_warnings
)

target_include_directories(stack_counting PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(stack_counting PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(test)