cmake_minimum_required(VERSION 2.6)
project(shell)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(shell 
main.cpp
)

target_link_libraries(shell neat)

add_custom_command(
        TARGET shell POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/shell
                ../../bin/shell)
