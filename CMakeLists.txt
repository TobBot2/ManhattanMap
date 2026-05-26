cmake_minimum_required(VERSION 4.1)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(ManhattanMap)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 23)

pico_sdk_init()

add_executable(ManhattanMap
        main.cpp

        Line.cpp
        Line.h
        Light.h
        vmath.h
)

# generate .pio.h file
pico_generate_pio_header(ManhattanMap ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio)

# allow CLion to find generated .pio.h file
target_include_directories(ManhattanMap PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

# enable usb output, disable uart output
pico_enable_stdio_usb(ManhattanMap 1)
pico_enable_stdio_uart(ManhattanMap 0)

target_link_libraries(ManhattanMap
        pico_stdlib
        hardware_pio
)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(ManhattanMap)