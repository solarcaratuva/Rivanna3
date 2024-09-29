include_directories(include)

add_subdirectory(lib)

add_executable(PowerBoard src/main.cpp)

target_link_libraries(DriverBoard PRIVATE mbed-os-lib Common-lib PowerBoard-lib)

mbed_set_post_build(PowerBoard)