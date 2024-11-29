add_library(WheelBoard-lib
    src/WheelCANInterface.cpp

)

target_include_directories(WheelBoard-lib PUBLIC include)

target_link_libraries(WheelBoard-lib mbed-os-lib Common-lib)