cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(zephyr_qemi_networking_demo)

# message(STATUS "LDD $PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}")
# message(STATUS "LDD $PROJECT_BINARY_DIR ${PROJECT_BINARY_DIR}")

# library myzephyrlib
set(MYZEPHYRLIB_PATH "${PROJECT_SOURCE_DIR}/lib/myzephyrlib")
add_subdirectory(${MYZEPHYRLIB_PATH})
zephyr_include_directories(lib/myzephyrlib/include)
target_link_libraries(myzephyrlib INTERFACE kernel)

# library simplelib
set(SIMPLELIB_PATH "${PROJECT_SOURCE_DIR}/lib/simplelib")
add_subdirectory(${SIMPLELIB_PATH})
zephyr_include_directories(lib/simplelib/include)


# application
file(GLOB_RECURSE app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})
zephyr_include_directories(src)
zephyr_include_directories(src/tls_config)

# link external library to application (as private to prevent cyclic dependency)
#   see : https://github.com/zephyrproject-rtos/zephyr/issues/31613#issuecomment-768156889
target_link_libraries(app PUBLIC simplelib)
target_link_libraries(app PRIVATE myzephyrlib)