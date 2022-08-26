cmake_minimum_required(VERSION 3.0)
project(snake5)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(snake5
snake5.cpp
system.cpp
mutations.cpp
pop.cpp
)

target_link_libraries(snake5 gacommon logger pthread)
