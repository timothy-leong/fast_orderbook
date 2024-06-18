cmake_minimum_required(VERSION 3.28.2)
project(fast_ob CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(dependencies.cmake)

add_library(fast_ob STATIC orderbook.cpp update_dump.cpp)
target_link_libraries(fast_ob phmap)