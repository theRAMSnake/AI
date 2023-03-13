cmake_minimum_required(VERSION 3.0)
project(sori)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(sori
    sori.cpp
    components.cpp
    pop.cpp
    data.cpp
    environment.cpp
    database.cpp
)

target_link_libraries(sori gacommon logger pthread sqlite3 profiler)

add_subdirectory(pop_reader)
