cmake_minimum_required(VERSION 3.3)
project(Project_2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    half_fit.c

    main.c
    type.h)

add_executable(Project_2 ${SOURCE_FILES})