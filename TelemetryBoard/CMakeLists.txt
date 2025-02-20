include_directories(include)

add_executable(TelemetryBoard src/main.cpp)

target_link_libraries(TelemetryBoard PRIVATE mbed-os-lib Common-lib)

mbed_set_post_build(TelemetryBoard)