cmake_minimum_required(VERSION 3.0)
project(logger)

LIST(APPEND CMAKE_MODULE_PATH "..")

add_library(logger 
Logger.cpp
)

target_link_libraries(logger pthread)
