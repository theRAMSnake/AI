cmake_minimum_required(VERSION 3.0)
project(host)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(host
main.cpp
Host.cpp
Engines.cpp
Playgrounds.cpp
)

find_package(Boost COMPONENTS filesystem REQUIRED)
target_link_libraries(host gacommon snake4 playgrounds ${Boost_LIBRARIES})

add_custom_command(
        TARGET host POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/host
                ../../bin/host)
