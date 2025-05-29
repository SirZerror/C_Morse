# Minimum CMake version
cmake_minimum_required(VERSION 3.17)

# Project info
project(
    morse
    VERSION 1.0
    DESCRIPTION "Morse Code Utility"
    LANGUAGES C
)

# C standard
set(CMAKE_C_STANDARD 17)

# Flags for debug and release
set(CMAKE_C_FLAGS_DEBUG_INIT "-Wall -g")
set(CMAKE_C_FLAGS_RELEASE_INIT "-Wall -O3 -s")

# Include directory

# Source files

# Executable
add_executable(${PROJECT_NAME} morse.c)

