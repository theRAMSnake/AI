cmake_minimum_required(VERSION 2.6)
project(gui)

LIST(APPEND CMAKE_MODULE_PATH "..")

include_directories("..")

add_executable(gui
main.cpp
TetrisPG.cpp
MainMenuCtrl.cpp
ControlPanelCtrl.cpp
ProjectManager.cpp
CheckpointPG.cpp
Trainer.cpp
panels/HistoryPanel.cpp
panels/PopulationPanel.cpp
panels/RawOutPanel.cpp
panels/SpeciePanel.cpp
widgets/plot.cpp
widgets/propertygrid.cpp
widgets/pgitems.cpp
widgets/nn_view.cpp
projects/NeatProject.cpp
projects/NeatPopulation.cpp
)

target_link_libraries(gui nana Xcursor neat tetris logger)

add_custom_command(
        TARGET gui POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_BINARY_DIR}/gui
                ../../bin/gui)
