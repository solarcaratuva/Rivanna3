include_directories(include)

add_subdirectory(lib)

add_executable(DriverBoard src/main.cpp)

target_link_libraries(DriverBoard PRIVATE mbed-os-lib Common-lib DriverBoard-lib)

mbed_set_post_build(DriverBoard)
