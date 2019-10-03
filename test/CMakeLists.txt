cmake_minimum_required(VERSION 2.6)
project(test)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(test 
CrossoverTest.cpp
MutationTest.cpp
SpeciationTest.cpp
PopulationTest.cpp
NeuroNetTest.cpp
)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

target_link_libraries(test neat ${Boost_LIBRARIES})

add_custom_command(
        TARGET test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/test
                ../../bin/test)
