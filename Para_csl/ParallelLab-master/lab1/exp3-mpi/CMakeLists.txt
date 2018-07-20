cmake_minimum_required(VERSION 3.10)
project(ParallelLab1_OpenMPI)

include (FindMPI)

if (NOT MPI_FOUND)
    message(FATAL_ERROR "No MPI implementation has been found.")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_CXX_COMPILER_FLAGS}")
set(LINK_FLAGS "${LINK_FLAGS} ${MPI_CXX_LINK_FLAGS}")

add_executable(vector_add vector_add.cpp)

target_link_libraries(vector_add ${MPI_LIBRARIES})    
include_directories(${MPI_INCLUDE_PATH})
