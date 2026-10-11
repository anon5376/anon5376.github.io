# SPDX-License-Identifier: Unlicense
# Adapted from LibTomMath demo/cmake_subproject/CMakeLists.txt in PR #612.
cmake_minimum_required(VERSION 3.10)
project(libtommath_parent C)

add_subdirectory("${LIBTOMMATH_SOURCE}" libtommath)
