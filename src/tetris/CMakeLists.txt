cmake_minimum_required(VERSION 3.0)
project(tetris)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("/usr/include/SDL/")

add_library(tetris 
Board.cpp
Game.cpp
Pieces.cpp
Tetris.cpp
)

target_link_libraries(tetris SDL SDL_gfx)
