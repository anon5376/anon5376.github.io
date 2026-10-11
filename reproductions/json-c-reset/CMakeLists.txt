# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.13)
project(json_c_reset_probe C)

add_subdirectory("${JSON_C_SOURCE}" json-c)
add_executable(json-c-reset-probe probe.c)
target_compile_features(json-c-reset-probe PRIVATE c_std_99)
target_link_libraries(json-c-reset-probe PRIVATE json-c)
