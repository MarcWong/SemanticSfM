cmake_minimum_required(VERSION 2.8)

project(graclus  C CXX)
set(CMAKE_CXX_STANDARD 11)
add_subdirectory(metisLib)
add_subdirectory(multilevelLib)
add_subdirectory(programs)
