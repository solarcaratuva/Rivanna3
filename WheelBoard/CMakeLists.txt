include_directories(include)

add_subdirectory(lib)

add_executable(WheelBoard src/main.cpp)

target_link_libraries(WheelBoard PRIVATE mbed-os-lib Common-lib WheelBoard-lib)

mbed_set_post_build(WheelBoard)