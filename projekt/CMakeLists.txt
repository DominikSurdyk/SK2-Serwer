cmake_minimum_required(VERSION 3.13)
project(projekt)

set(CMAKE_CXX_STANDARD 14)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall")
add_executable(projekt main.cpp Game.cpp Game.h)