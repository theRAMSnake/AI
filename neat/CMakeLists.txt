cmake_minimum_required(VERSION 2.6)
project(neat)

LIST(APPEND CMAKE_MODULE_PATH "..")

add_library(neat 
genom.cpp
neat.cpp
population.cpp
rng.cpp
InnovationHistory.cpp
neuro_net.cpp
activation.cpp
)

target_link_libraries(neat logger pthread)
