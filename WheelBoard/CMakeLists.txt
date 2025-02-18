include_directories(include)

add_subdirectory(lib)

add_subdirectory(touchgfx)

add_executable(WheelBoard src/main.cpp)

target_link_libraries(WheelBoard PRIVATE mbed-os-lib Common-lib WheelBoard-lib touchgfx-lib)

target_include_directories(WheelBoard PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/touchgfx/Middlewares/ST/touchgfx_components/gpu2d/TouchGFXNema/include
    ${CMAKE_CURRENT_SOURCE_DIR}/touchgfx/Middlewares/ST/touchgfx/framework/include
    ${CMAKE_CURRENT_SOURCE_DIR}/touchgfx/Middlewares/ST/touchgfx_components/gpu2d/NemaGFX/include
    ${CMAKE_CURRENT_SOURCE_DIR}/touchgfx/Middlewares/ST/touchgfx/framework/include
    ${CMAKE_CURRENT_SOURCE_DIR}/touchgfx/Middlewares/ST/touchgfx/framework/include/touchgfx
    ${CMAKE_CURRENT_SOURCE_DIR}/touchgfx/TouchGFX/generated/images/include/images
)

mbed_set_post_build(WheelBoard)