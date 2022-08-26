cmake_minimum_required(VERSION 3.0)

project(AI)

include_directories(.)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++20 -O2")

add_subdirectory(logger)
add_subdirectory(host)
add_subdirectory(neat)
add_subdirectory(snake4)
#add_subdirectory(snake5)
add_subdirectory(SORI)
add_subdirectory(test)
add_subdirectory(tetris)
#add_subdirectory(GUI)
add_subdirectory(gacommon)
add_subdirectory(playgrounds)
