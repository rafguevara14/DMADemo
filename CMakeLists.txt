cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(dma_demo C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_EXAMPLES_PATH ${PROEJECT_SOURCE_DIR})

pico_sdk_init()

add_executable(dma_demo 
			dma_demo.c
			)

pico_enable_stdio_usb(dma_demo 1)
pico_enable_stdio_uart(dma_demo 0)

target_link_libraries(dma_demo pico_stdlib hardware_pwm hardware_adc)

pico_add_extra_outputs(dma_demo)
