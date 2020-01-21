cmake_minimum_required(VERSION 2.6)
project(snakega)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(snakega 
genom.cpp
snakega.cpp
pop.cpp
exploitation.cpp
)

target_link_libraries(snakega neuroevolution logger pthread)
