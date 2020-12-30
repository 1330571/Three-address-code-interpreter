cmake_minimum_required(VERSION 3.17)
project(MidCodeInterpreter)

set(CMAKE_CXX_STANDARD 17)

add_executable(MidCodeInterpreter main.cpp)
add_executable(MidCodeOptimizer midCodeOptimizer.cpp)