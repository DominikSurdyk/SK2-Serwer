cmake_minimum_required(VERSION 3.13)
project(projekt)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall")
add_executable(projekt main.cpp Game.cpp Game.h)