add_library(PowerBoard-lib
    src/PowerCANInterface.cpp
    src/MotorControllerCANInterface.cpp
    src/MotorInterface.cpp
    src/ReadPedals.cpp
    src/CruiseControl.cpp
)

target_include_directories(PowerBoard-lib PUBLIC include)

target_link_libraries(PowerBoard-lib mbed-os-lib Common-lib)
