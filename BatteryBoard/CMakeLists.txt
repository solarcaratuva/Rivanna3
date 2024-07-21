include_directories(include)

add_subdirectory(lib)

add_executable(BatteryBoard src/main.cpp)

target_link_libraries(BatteryBoard PRIVATE mbed-os-lib Common-lib BatteryBoard-lib)

mbed_set_post_build(BatteryBoard)
    