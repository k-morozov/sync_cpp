cmake_minimum_required(VERSION 3.25)
project(sync-cpp)

set(CMAKE_CXX_STANDARD 20)

option(CHECK_ADDRESS "check address" ON)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_macros.html
    add_compile_options(-D_GLIBCXX_ASSERTIONS -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_LIBCPP_DEBUG=1)

    add_compile_options(-O0)

    if(CHECK_ADDRESS)
        message("Check with address sanitize")
        add_compile_options(-fsanitize=address)
        add_link_options(-fsanitize=address)
    else()
        message("Check with thread sanitize")
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif ()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
endif ()

# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
# https://clang.llvm.org/docs/DiagnosticsReference.html
add_compile_options(-Wall -Wextra -Wpedantic)

get_directory_property(CMAKE_CXX_FLAGS_LIST COMPILE_OPTIONS)
list(JOIN CMAKE_CXX_FLAGS_LIST " " CMAKE_CXX_FLAGS)
message("Compiler flags: ${CMAKE_CXX_FLAGS}")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(src)
include_directories(tests)

add_subdirectory(src)
add_subdirectory(tests)
#add_subdirectory(example)
