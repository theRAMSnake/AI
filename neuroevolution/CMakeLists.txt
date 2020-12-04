cmake_minimum_required(VERSION 2.6)
project(neuroevolution)

LIST(APPEND CMAKE_MODULE_PATH "..")

add_library(neuroevolution 
neuro_net.cpp
neuro_net2.cpp
activation.cpp
rng.cpp
)

target_link_libraries(neuroevolution logger)
