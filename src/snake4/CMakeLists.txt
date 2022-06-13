cmake_minimum_required(VERSION 3.0)
project(snake4)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(snake4
snake4.cpp
system.cpp
mutations.cpp
pop.cpp
)

target_link_libraries(snake4 gacommon logger pthread)
