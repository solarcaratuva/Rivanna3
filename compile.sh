#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Compiling for Rivanna3 boards"
    mbed-tools compile -m NUCLEO_F413ZH -t GCC_ARM
    mbed-tools compile -m POWER_BOARD -t GCC_ARM
    exit $?
fi

if [ "$1" == "riv2s" ]; then
    echo "Compiling for Rivanna2S boards"
    mbed-tools compile -m UVA_SOLAR_CAR -t GCC_ARM
    exit $?
fi

if [ "$1" == "telem" ]; then
    echo "Compiling for Telemetry board"
    mbed-tools compile -m TELEMETRY_BOARD -t GCC_ARM
    exit $?
fi

if [ "$1" == "hil743" ]; then
    echo "Compiling for Hardware-in-Loop Testing board (Nucleo H743ZI2)"
    mbed-tools compile -m NUCLEO_H743ZI2 -t GCC_ARM
    exit $?
fi

if [ "$1" == "hil413" ]; then
    echo "Compiling for Hardware-in-Loop Testing board (Nucleo F413ZH)"
    mbed-tools compile -m NUCLEO_F413ZH -t GCC_ARM
    exit $?
fi




echo "did not recognize '$1'"
exit 1