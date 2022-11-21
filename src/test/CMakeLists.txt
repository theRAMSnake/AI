cmake_minimum_required(VERSION 3.0)
project(test)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(test 
    #NaturalSelectionTest.cpp
    #SnakeGATest.cpp
    SoriCompsTest.cpp
    #SoriMutationsTest.cpp
    #SoriSaveLoadTest.cpp
    SoriDataTest.cpp
    SoriConvergenceTest.cpp
    #SoriBrokenPopsTest.cpp
    #CrossoverTest.cpp
    #MutationTest.cpp
    #NeuroNetTest.cpp
    #RNNTest.cpp
    #SpecieTest.cpp
#SaveLoadStateTest.cpp
)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

target_link_libraries(test neat snake4 sori ${Boost_LIBRARIES})

add_custom_command(
        TARGET test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/test
                ../../bin/test)
