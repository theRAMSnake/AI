cmake_minimum_required(VERSION 3.0)
project(sori)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(sori
    sori.cpp
    pop.cpp
    environment.cpp
    components.cpp
)

target_link_libraries(sori gacommon logger pthread)
