cmake_minimum_required(VERSION 3.0)
project(dng)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(dng
    drawing.cpp
    geometry.cpp
    shape.cpp
)

target_link_libraries(dng sori)
