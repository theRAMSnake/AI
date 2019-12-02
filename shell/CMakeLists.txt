cmake_minimum_required(VERSION 2.6)
project(shell)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("/usr/include/SDL/" "..")

add_executable(shell 
main.cpp
NeatController.cpp
TetrisPG.cpp
)

target_link_libraries(shell neat tetris)

add_custom_command(
        TARGET shell POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/shell
                ../../bin/shell)
