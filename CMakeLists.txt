cmake_minimum_required(VERSION 3.1)
project(AOT_OPTIMIZER LANGUAGES CXX)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_EXTENSIONS ON)
include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(lib)
add_subdirectory(tools)
