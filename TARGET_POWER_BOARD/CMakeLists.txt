add_library(mbed-power-board INTERFACE)

target_sources(mbed-power-board
    INTERFACE
        PeripheralPins.c
        system_clock.c
)

target_include_directories(mbed-power-board
    INTERFACE
        .
)

target_link_libraries(mbed-power-board INTERFACE mbed-stm32g473xe)