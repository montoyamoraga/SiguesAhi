# SiguesAhi como biblioteca del pico sdk, para pico w y pico 2 w.
#
# uso, en el CMakeLists.txt de tu programa, después de pico_sdk_init():
#   add_subdirectory(ruta/a/SiguesAhi siguesahi)
#   target_link_libraries(mi_programa siguesahi pico_stdlib)
#
# ver pico/ejemplos/ para un programa completo

cmake_minimum_required(VERSION 3.13)

if (NOT COMMAND pico_sdk_init)
    message(FATAL_ERROR "SiguesAhi: incluye pico_sdk_import.cmake y llama a "
                        "pico_sdk_init() antes de add_subdirectory(SiguesAhi)")
endif()

option(SIGUES_AHI_CONFIGURACION
       "usar lwipopts.h y mbedtls_config.h de SiguesAhi" ON)

# ArduinoJson es solo encabezados y no depende de arduino
if (NOT TARGET ArduinoJson)
    include(FetchContent)
    FetchContent_Declare(ArduinoJson
        GIT_REPOSITORY https://github.com/bblanchon/ArduinoJson.git
        GIT_TAG v7.4.3
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(ArduinoJson)
endif()

set(SIGUES_AHI_SRC ${CMAKE_CURRENT_LIST_DIR}/src)

# biblioteca INTERFACE, como las del pico sdk: se compila junto al programa,
# con su misma configuración de lwIP y mbedTLS
add_library(siguesahi INTERFACE)

target_sources(siguesahi INTERFACE
    ${SIGUES_AHI_SRC}/siguesahi/Http.cpp
    ${SIGUES_AHI_SRC}/siguesahi/Instrumento.cpp
    ${SIGUES_AHI_SRC}/siguesahi/SalidaParpadeo.cpp
    ${SIGUES_AHI_SRC}/siguesahi/SalidaTexto.cpp
    ${SIGUES_AHI_SRC}/siguesahi/Tipos.cpp
    ${SIGUES_AHI_SRC}/siguesahi/Vigilante.cpp
    ${SIGUES_AHI_SRC}/siguesahi/Wikidata.cpp
    ${SIGUES_AHI_SRC}/siguesahi/pico/RedPico.cpp
)

target_include_directories(siguesahi INTERFACE ${SIGUES_AHI_SRC})

if (SIGUES_AHI_CONFIGURACION)
    target_include_directories(siguesahi INTERFACE
        ${SIGUES_AHI_SRC}/siguesahi/pico/configuracion)
endif()

target_compile_definitions(siguesahi INTERFACE SIGUES_AHI_PICO_SDK=1)

target_link_libraries(siguesahi INTERFACE
    ArduinoJson
    hardware_watchdog
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_mbedtls
    pico_lwip_sntp
    pico_mbedtls
    pico_stdlib
)
