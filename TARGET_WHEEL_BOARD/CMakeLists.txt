# Copyright (c) 2025 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(mbed-wheel-board INTERFACE)

target_sources(mbed-wheel-board
    INTERFACE
        PeripheralPins.c
)

target_include_directories(mbed-wheel-board
    INTERFACE
        .
)

target_link_libraries(mbed-wheel-board INTERFACE mbed-stm32u5a9xj)