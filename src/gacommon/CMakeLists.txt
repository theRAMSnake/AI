cmake_minimum_required(VERSION 3.0)
project(gacommon)

LIST(APPEND CMAKE_MODULE_PATH "..")

add_library(
gacommon 
neuro_net2.cpp
activation.cpp
rng.cpp
)

find_package(Boost COMPONENTS serialization REQUIRED)

target_link_libraries(gacommon logger ${Boost_LIBRARIES} profiler)
