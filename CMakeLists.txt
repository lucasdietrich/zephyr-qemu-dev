cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(zephyr_qemi_networking_demo)

FILE(GLOB app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})
include_directories(inc)