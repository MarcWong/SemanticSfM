cmake_minimum_required(VERSION 2.8)
project(TEST_FMINCON)

set(CMAKE_CXX_FLAGS "-std=c++11")

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(/usr/local/MATLAB/R2016b/bin/glnxa64/)

add_subdirectory(${PROJECT_SOURCE_DIR}/src bin)
