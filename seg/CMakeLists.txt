cmake_minimum_required(VERSION 2.6)
project(seg)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_library(seg 
interpreter.cpp
functions.cpp
graph_traverser.cpp
graph.cpp
mutator.cpp
algorithm.cpp
agent.cpp
)

target_link_libraries(seg neuroevolution logger pthread)
