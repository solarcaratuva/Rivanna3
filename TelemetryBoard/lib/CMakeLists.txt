add_library(TelemetryBoard-lib
    src/TelemetryCANInterface.cpp
    src/EEPROMDriver.cpp
)

target_include_directories(TelemetryBoard-lib PUBLIC include)

target_link_libraries(TelemetryBoard-lib mbed-os-lib Common-lib)
