cmake_minimum_required(VERSION 2.6)
project(neat)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(neat 
genom.cpp
neat.cpp
population.cpp
rng.cpp
InnovationHistory.cpp
Substrate3D.cpp
)

target_link_libraries(neat neuroevolution logger pthread)
