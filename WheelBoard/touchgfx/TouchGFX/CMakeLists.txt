file(GLOB_RECURSE TOUCHGFX_SOURCES
    App/*.cpp
    generated/*.cpp
    gui/*.cpp
    target/*.cpp
)

list(FILTER TOUCHGFX_SOURCES EXCLUDE REGEX "simulator")

add_library(touchgfx-lib STATIC ${TOUCHGFX_SOURCES})

target_link_libraries(touchgfx-lib 
    PRIVATE
        mbed-os-lib
)

target_include_directories(touchgfx-lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/App
    ${CMAKE_CURRENT_SOURCE_DIR}/config
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/gui
    ${CMAKE_CURRENT_SOURCE_DIR}/target
    ${CMAKE_CURRENT_SOURCE_DIR}/target/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/gui_generated/include
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/include/gui/common
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/fonts/include/fonts
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/include/gui/model
    # ${CMAKE_CURRENT_SOURCE_DIR}/generated/simulator/include/simulator
    # ${CMAKE_CURRENT_SOURCE_DIR}/generated/simulator/src
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/touchgfx/framework/include
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/include
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/texts/include
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/images/include
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/fonts/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Core/Inc
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/touchgfx_components/gpu2d/TouchGFXNema/include
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/touchgfx_components/gpu2d/NemaGFX/include
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Drivers/STM32U5xx_HAL_Driver/Inc
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Drivers/CMSIS/Device/ST/STM32U5xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/ST/touchgfx/3rdparty/libjpeg/include
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../../mbed-os/targets/TARGET_STM/TARGET_STM32U5/STM32Cube_FW/CMSIS
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../../mbed-os/cmsis/tests/UNITTESTS/doubles
    # ${CMAKE_CURRENT_SOURCE_DIR}/../../../mbed-os/targets/TARGET_STM/TARGET_STM32U5/STM32Cube_FW/STM32U5xx_HAL_Driver
    # ${CMAKE_CURRENT_SOURCE_DIR}/../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
)


