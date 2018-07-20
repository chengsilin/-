cmake_minimum_required(VERSION 3.10)
project(ParallelLab1_OpenMP)

include (FindOpenMP)

if (OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}") # Enable OpenMP for C++
else()
    message("WARNING: OpenMP not supported!")
endif()

add_executable (vector_add vector_add.cpp)