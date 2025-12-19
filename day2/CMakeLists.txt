# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(day2 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# turn on warnings
add_compile_options(-Wall -Wextra -Wpedantic -Wconversion)

# FetchContent
include(FetchContent)

FetchContent_Declare(
    etl
    GIT_REPOSITORY https://github.com/ETLCPP/etl
    GIT_TAG 20.44.2
)

FetchContent_MakeAvailable(etl)
add_library(etl_profile INTERFACE)
target_sources(
    etl_profile
    PUBLIC FILE_SET HEADERS BASE_DIRS ../include FILES ../include/etl_profile.h
)
target_link_libraries(etl INTERFACE etl_profile)

# Add executable. Default name is the project name, version 0.1

add_executable(day2 day2.cpp )

pico_set_program_name(day2 "day2")
pico_set_program_version(day2 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(day2 0)
pico_enable_stdio_usb(day2 1)

# Add the standard library to the build
target_link_libraries(day2
        pico_stdlib
        etl::etl
)

# Add the standard include files to the build
target_include_directories(day2 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ../include
)

pico_add_extra_outputs(day2)

