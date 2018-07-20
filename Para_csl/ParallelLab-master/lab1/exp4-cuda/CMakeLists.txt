cmake_minimum_required(VERSION 3.10)
project(ParallelLab1_CUDA)

enable_language(CUDA)

add_executable(vector_add src/vector_add.cu)