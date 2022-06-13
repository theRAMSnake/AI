cmake_minimum_required(VERSION 3.0)
project(neat)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(neat 
genom.cpp
neat.cpp
population.cpp
InnovationHistory.cpp
)

target_link_libraries(neat gacommon logger pthread)
