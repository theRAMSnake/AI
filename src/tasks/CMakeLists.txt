cmake_minimum_required(VERSION 3.0)
project(tasks)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(tasks
    manager.cpp
    ObjectDetection.cpp
)

target_link_libraries(tasks sori )
