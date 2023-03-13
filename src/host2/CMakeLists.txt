cmake_minimum_required(VERSION 3.0)
project(host2)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(host2
main.cpp
sori.cpp
Host.cpp
)

find_package(Boost COMPONENTS filesystem REQUIRED)
target_link_libraries(host2 gacommon dng sori tasks ${Boost_LIBRARIES} profiler)

add_custom_command(
        TARGET host2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/host2
                ../../bin/host2)
