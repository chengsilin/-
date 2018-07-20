cmake_minimum_required(VERSION 3.10)
project(ParallelLab3)

include (FindOpenMP)

if (OPENMP_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}") # Enable OpenMP for C++
else()
    message("WARNING: OpenMP not supported!")
endif()

add_executable(morph main.cpp ../morph-linear/morphlib.h ../morph-linear/morphlib.cpp)