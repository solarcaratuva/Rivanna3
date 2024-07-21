$firmwarePath = "cmake_build/UVA_SOLAR_CAR/develop/GCC_ARM/DriverBoard/DriverBoard.bin"

st-flash.exe --connect-under-reset --reset write $firmwarePath 0x8000000