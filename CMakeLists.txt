# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name
project(CompProgramming2Project)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add the include directory for headers
include_directories(
    ${CMAKE_SOURCE_DIR}/HeaderFiles
    ${CMAKE_SOURCE_DIR}
)

# Specify all the source files
file(GLOB SRC_FILES "HeaderFiles/*.cpp")

# Add the executable target
add_executable(CompProjectExec ${SRC_FILES})