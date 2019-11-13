cmake_minimum_required(VERSION 2.6)
project(gui)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(gui 
main.cpp
TetrisPG.cpp
NeatController.cpp
)

target_link_libraries(gui nana Xcursor neat tetris)

add_custom_command(
        TARGET gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/gui
                ../../bin/gui)
