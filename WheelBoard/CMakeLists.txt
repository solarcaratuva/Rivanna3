include_directories(include)

add_subdirectory(lib)

add_subdirectory(touchgfx)

add_executable(WheelBoard src/main.cpp)

target_link_libraries(WheelBoard PRIVATE mbed-os-lib Common-lib WheelBoard-lib touchgfx-lib)

mbed_set_post_build(WheelBoard)

target_include_directories(WheelBoard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/touchgfx/include
)

mbed_set_post_build(WheelBoard)