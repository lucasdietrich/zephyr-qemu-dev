cmake_minimum_required(VERSION 3.20.0)

set(ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules/lua)

find_package(Zephyr)
project(zephyr_qemu_lua_demo)

FILE(GLOB app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})