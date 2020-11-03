cmake_minimum_required(VERSION 2.6)
project(profile)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(profile 
main.cpp
)

target_link_libraries(profile neat neuroevolution)

add_custom_command(
        TARGET profile POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/profile
                ../../bin/profile)
