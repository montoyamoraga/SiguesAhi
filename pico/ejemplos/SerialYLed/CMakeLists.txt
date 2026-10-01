# ejemplo de SiguesAhi con el pico sdk
#
# compilar, con PICO_SDK_PATH apuntando al pico sdk:
#   cmake -S . -B build -DPICO_BOARD=pico2_w   (o pico_w)
#   cmake --build build
# y copiar build/serial_y_led.uf2 a la placa en modo BOOTSEL

cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w CACHE STRING "placa: pico_w o pico2_w")

include(pico_sdk_import.cmake)

project(serial_y_led C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

# la biblioteca está tres carpetas más arriba
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../.. siguesahi)

add_executable(serial_y_led main.cpp)

# para encontrar secret.h
target_include_directories(serial_y_led PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(serial_y_led siguesahi pico_stdlib)

# mensajes por USB, no por UART
pico_enable_stdio_usb(serial_y_led 1)
pico_enable_stdio_uart(serial_y_led 0)

pico_add_extra_outputs(serial_y_led)
