add_library(PowerBoard-lib
    src/PowerCANInterface.cpp
    src/MotorInterface.cpp
    src/ReadFiles.cpp
)

target_include_directories(PowerBoard-lib PUBLIC include)

target_link_libraries(PowerBoard-lib mbed-os-lib Common-lib)
