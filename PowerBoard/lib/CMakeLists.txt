add_library(PowerBoard-lib
    src/PowerCANInterface.cpp
)

target_include_directories(PowerBoard-lib PUBLIC include)

target_link_libraries(PowerBoard-lib mbed-os-lib Common-lib)