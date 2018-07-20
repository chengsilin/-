cmake_minimum_required(VERSION 3.10)
project(ParallelLab5)

enable_language(CUDA)

add_executable(morph main.cu ../morph-linear/morphlib.cpp ../morph-linear/morphlib.h)