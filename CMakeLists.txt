cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(arven_user C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        )

add_executable(arven_user
arven_user.c
)

pico_enable_stdio_usb(arven_user 1)
pico_enable_stdio_uart(arven_user 1)
pico_add_extra_outputs(arven_user)

# add our custom libraries
add_subdirectory(dwm1001)
add_subdirectory(web)

target_link_libraries(arven_user 
    dwm1001
    web
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    pico_stdlib)

target_include_directories(arven_user PRIVATE
    "${PROJECT_SOURCE_DIR}/dwm1001"
    "${PROJECT_SOURCE_DIR}/web")
