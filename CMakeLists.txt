cmake_minimum_required(VERSION 3.20.0)

set(ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules/lua)
set(DTC_OVERLAY_FILE "qemu_x86.overlay")

find_package(Zephyr)
project(zephyr_qemu_lua_demo)

set(embedded_lua_scripts "")
file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/src/lua_scripts/*.lua")
foreach(file ${files})
    get_filename_component(filename ${file} NAME)
    get_filename_component(filepath ${file} PATH)
    message(STATUS "embedding ${filepath}/${filename} into elf")
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/embluaobj/${filename}.o
        WORKING_DIRECTORY ${filepath}
        COMMAND ${CMAKE_OBJCOPY}
        ARGS -I binary --output-target elf32-i386 --binary-architecture i386 ${filename} ${CMAKE_CURRENT_BINARY_DIR}/embluaobj/${filename}.o
    )
    # Find a way to automatically choose output-target and binary-architecture
    #   e.g. using : $<TARGET_PROPERTY:bintools,elfconvert_flag_outtarget>
    list(APPEND embedded_lua_scripts ${CMAKE_CURRENT_BINARY_DIR}/embluaobj/${filename}.o)
endforeach()

FILE(GLOB app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources} ${embedded_lua_scripts})

zephyr_linker_sources(SECTIONS iterables.ld)