# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/snake/AI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/snake/AI/build

# Include any dependencies generated for this target.
include neat/CMakeFiles/neat.dir/depend.make

# Include the progress variables for this target.
include neat/CMakeFiles/neat.dir/progress.make

# Include the compile flags for this target's objects.
include neat/CMakeFiles/neat.dir/flags.make

neat/CMakeFiles/neat.dir/genom.cpp.o: neat/CMakeFiles/neat.dir/flags.make
neat/CMakeFiles/neat.dir/genom.cpp.o: ../neat/genom.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/snake/AI/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object neat/CMakeFiles/neat.dir/genom.cpp.o"
	cd /home/snake/AI/build/neat && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/neat.dir/genom.cpp.o -c /home/snake/AI/neat/genom.cpp

neat/CMakeFiles/neat.dir/genom.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/neat.dir/genom.cpp.i"
	cd /home/snake/AI/build/neat && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/snake/AI/neat/genom.cpp > CMakeFiles/neat.dir/genom.cpp.i

neat/CMakeFiles/neat.dir/genom.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/neat.dir/genom.cpp.s"
	cd /home/snake/AI/build/neat && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/snake/AI/neat/genom.cpp -o CMakeFiles/neat.dir/genom.cpp.s

neat/CMakeFiles/neat.dir/neat.cpp.o: neat/CMakeFiles/neat.dir/flags.make
neat/CMakeFiles/neat.dir/neat.cpp.o: ../neat/neat.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/snake/AI/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object neat/CMakeFiles/neat.dir/neat.cpp.o"
	cd /home/snake/AI/build/neat && /usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/neat.dir/neat.cpp.o -c /home/snake/AI/neat/neat.cpp

neat/CMakeFiles/neat.dir/neat.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/neat.dir/neat.cpp.i"
	cd /home/snake/AI/build/neat && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/snake/AI/neat/neat.cpp > CMakeFiles/neat.dir/neat.cpp.i

neat/CMakeFiles/neat.dir/neat.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/neat.dir/neat.cpp.s"
	cd /home/snake/AI/build/neat && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/snake/AI/neat/neat.cpp -o CMakeFiles/neat.dir/neat.cpp.s

# Object files for target neat
neat_OBJECTS = \
"CMakeFiles/neat.dir/genom.cpp.o" \
"CMakeFiles/neat.dir/neat.cpp.o"

# External object files for target neat
neat_EXTERNAL_OBJECTS =

neat/libneat.a: neat/CMakeFiles/neat.dir/genom.cpp.o
neat/libneat.a: neat/CMakeFiles/neat.dir/neat.cpp.o
neat/libneat.a: neat/CMakeFiles/neat.dir/build.make
neat/libneat.a: neat/CMakeFiles/neat.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/snake/AI/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libneat.a"
	cd /home/snake/AI/build/neat && $(CMAKE_COMMAND) -P CMakeFiles/neat.dir/cmake_clean_target.cmake
	cd /home/snake/AI/build/neat && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/neat.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
neat/CMakeFiles/neat.dir/build: neat/libneat.a

.PHONY : neat/CMakeFiles/neat.dir/build

neat/CMakeFiles/neat.dir/clean:
	cd /home/snake/AI/build/neat && $(CMAKE_COMMAND) -P CMakeFiles/neat.dir/cmake_clean.cmake
.PHONY : neat/CMakeFiles/neat.dir/clean

neat/CMakeFiles/neat.dir/depend:
	cd /home/snake/AI/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/snake/AI /home/snake/AI/neat /home/snake/AI/build /home/snake/AI/build/neat /home/snake/AI/build/neat/CMakeFiles/neat.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : neat/CMakeFiles/neat.dir/depend

