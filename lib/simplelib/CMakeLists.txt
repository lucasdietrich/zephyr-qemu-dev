cmake_minimum_required(VERSION 3.13.1)

project(simplelib)

file(GLOB_RECURSE LIB_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

zephyr_library_named(simplelib)
zephyr_library_include_directories(include)
zephyr_library_sources(${LIB_SRC_FILES})
