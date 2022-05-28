cmake_minimum_required(VERSION 3.20.0)

set(BOARD qemu_x86)

find_package(Zephyr)
project(zephyr_qemi_networking_demo)


set(CANIOTLIB_PATH "${PROJECT_SOURCE_DIR}/libs/caniot-lib")
zephyr_library_named(caniotlib)
file(GLOB CANIOT_SOURCES "${CANIOTLIB_PATH}/src/**.c")
zephyr_library_sources(${CANIOT_SOURCES})
zephyr_library_include_directories(${CANIOTLIB_PATH}/include)
target_compile_definitions(caniotlib PRIVATE CONFIG_CANIOT_LOG_LEVEL=0) 
target_compile_definitions(caniotlib PRIVATE CONFIG_CANIOT_ARCH_AGNOSTIC=0)
# target_compile_definitions(caniotlib PRIVATE __ZEPHYR__) # Definition already applied

FILE(GLOB app_sources src/*.c*)
target_sources(app PRIVATE ${app_sources})

zephyr_include_directories(libs/caniot-lib/include)
zephyr_include_directories(src/tls_config)

target_link_libraries(app PUBLIC caniotlib)