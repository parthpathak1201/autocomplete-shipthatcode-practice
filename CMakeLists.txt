cmake_minimum_required(VERSION 3.20)
project(practice_simulator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
add_executable(simulator ${SOURCES})
