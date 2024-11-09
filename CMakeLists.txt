cmake_minimum_required(VERSION 3.10)  # Specify the CMake version
project(h1)                           # Project name (same as the output executable)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 20)

# Find all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

# Define the executable target
add_executable(h1 ${SOURCES})
