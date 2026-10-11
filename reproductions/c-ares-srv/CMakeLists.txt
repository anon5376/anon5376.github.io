# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.13)
project(cares_srv_probe C)

add_subdirectory("${CARES_SOURCE}" c-ares)
add_executable(c-ares-srv-probe probe.c)
set_target_properties(c-ares-srv-probe PROPERTIES
    C_STANDARD 90 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
target_link_libraries(c-ares-srv-probe PRIVATE c-ares::cares)
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(c-ares-srv-probe PRIVATE -Wall -Wextra -Werror -pedantic)
endif()
