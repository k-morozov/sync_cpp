cmake_minimum_required(VERSION 3.25)

include_directories(${CONAN_INCLUDE_DIRS_GTEST})
include_directories(${CONAN_INCLUDE_DIRS_BENCHMARK})

set(TEST_APP_NAME unit_test_stack_counting)

add_executable(${TEST_APP_NAME}
    main.cpp
    unit-tests.cpp
)

target_link_libraries(${TEST_APP_NAME}
        stack_counting
        ${CONAN_LIBS_GTEST}
        project_sanitizers
        project_warnings
)

enable_testing()