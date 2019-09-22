cmake_minimum_required(VERSION 2.6)

project(AI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++17")

add_subdirectory(neat)
add_subdirectory(test)