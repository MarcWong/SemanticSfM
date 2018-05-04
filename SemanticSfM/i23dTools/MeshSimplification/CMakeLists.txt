cmake_minimum_required(VERSION 2.8)
project(MeshSimplification)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(SOURCE_FILES main.cpp Feature.cpp Mesh.cpp MxHeap.cxx Simplify.cpp BilateralNormalFilter.cpp)
add_executable(MeshSimplification ${SOURCE_FILES})
