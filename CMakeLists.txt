cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(zephyr_qemi_networking_demo)

# message(STATUS "LDD $PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}")
# message(STATUS "LDD $PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}")

# library
set(SIMPLELIB_PATH "${PROJECT_SOURCE_DIR}/lib/simplelib")
add_subdirectory(${SIMPLELIB_PATH})
zephyr_include_directories(lib/simplelib/include)

# application
file(GLOB_RECURSE app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})
zephyr_include_directories(src)
zephyr_include_directories(src/tls_config)

# link external library to application
zephyr_link_libraries(simplelib)