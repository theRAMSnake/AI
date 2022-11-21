cmake_minimum_required(VERSION 3.0)
project(pop_reader)

LIST(APPEND CMAKE_MODULE_PATH "..")

add_executable(pop_reader
    main.cpp
)

target_link_libraries(pop_reader sori)
add_custom_command(
        TARGET pop_reader POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/pop_reader
                ../../../bin/pop_reader)
