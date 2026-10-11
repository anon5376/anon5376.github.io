# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.13)
project(hiredis_async_timeout_probe C)

set(DISABLE_TESTS ON CACHE BOOL "" FORCE)
set(ENABLE_SSL OFF CACHE BOOL "" FORCE)
set(ENABLE_NUGET OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory("${HIREDIS_SOURCE}" hiredis)
add_executable(hiredis-async-timeout-probe probe.c)
set_target_properties(hiredis-async-timeout-probe PROPERTIES
    C_STANDARD 99 C_STANDARD_REQUIRED ON C_EXTENSIONS OFF)
target_include_directories(hiredis-async-timeout-probe PRIVATE "${HIREDIS_SOURCE}")
target_link_libraries(hiredis-async-timeout-probe PRIVATE hiredis)
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(hiredis-async-timeout-probe PRIVATE -Wall -Wextra -Werror -pedantic)
endif()
