
cmake_minimum_required(VERSION 3.0)
project(tasksDemo)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(tasksDemo
    main.cpp
)

target_link_libraries(tasksDemo tasks dng ${Boost_LIBRARIES})

add_custom_command(
        TARGET tasksDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/tasksDemo
                ../../bin/tasksDemo)
