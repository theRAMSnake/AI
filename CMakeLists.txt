cmake_minimum_required(VERSION 2.6)

project(AI)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++17 -DNANA_FILESYSTEM_FORCE -pg")

add_subdirectory(logger)
add_subdirectory(neat)
add_subdirectory(snakega)
add_subdirectory(test)
add_subdirectory(tetris)
add_subdirectory(GUI)
add_subdirectory(neuroevolution)