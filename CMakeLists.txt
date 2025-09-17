cmake_minimum_required(VERSION 3.14)
project(sprint)
set(pname ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXPORT_COMPILE_COMMANDS ON)

add_executable(${pname} main.cpp)
target_include_directories(${pname} PRIVATE ./)