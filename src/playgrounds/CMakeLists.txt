cmake_minimum_required(VERSION 3.0)
project(playgrounds)

LIST(APPEND CMAKE_MODULE_PATH "..")
include_directories("..")

add_library(playgrounds
    #TetrisPG.cpp
LogicalPG.cpp
CalculatorPG.cpp
)

target_link_libraries(playgrounds neat logger )
