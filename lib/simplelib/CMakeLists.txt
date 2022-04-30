cmake_minimum_required(VERSION 3.13.1)

project(simplelib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

add_library(simplelib STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/slib.c
)
