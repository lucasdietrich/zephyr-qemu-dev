cmake_minimum_required(VERSION 3.20.0)

set(BOARD qemu_x86)

find_package(Zephyr)
project(zephyr_qemi_networking_demo)

FILE(GLOB app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})
include_directories(inc)

zephyr_include_directories(src/tls_config)