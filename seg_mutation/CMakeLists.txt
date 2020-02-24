cmake_minimum_required(VERSION 2.6)
project(seg_mutation)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(seg_mutation 
main.cpp
)

target_link_libraries(seg_mutation seg)

add_custom_command(
        TARGET seg_mutation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/seg_mutation
                ../../bin/seg_mutation)
