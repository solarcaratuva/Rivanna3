#include <mbed.h>
#include <rtos.h>
#include "main.h"
#include "ReadPedals.h"

#define SCALING_FACTOR  (3.3f/3.28f) // the pedal voltage is 3.28V max, but the AnalogIn pin reads 3.3V


// Reads the throttle pedal value and returns a uint16_t
uint16_t read_throttle() {
    float adjusted_throttle_input = throttle_pedal.read() * SCALING_FACTOR;
    if (adjusted_throttle_input <= 0.0f) { // should never happen
        return 0;
    } else if (adjusted_throttle_input >= 1.0f) { // should only happen if the hardware voltage divider is wrong
        return 256;
    } else {
        return (uint16_t)(adjusted_throttle_input * 256.0);
    }
}

// TODO Reads the brake pedal value and returns a uint16_t
uint16_t read_brake() {
    float adjusted_brake_input = brake_pedal.read() * SCALING_FACTOR;
    if (adjusted_brake_input <= 0.0f) { // should never happen
        return 0;
    } else if (adjusted_brake_input >= 1.0f) { // should only happen if the hardware voltage divider is wrong
        return 256;
    } else {
        return (uint16_t)(adjusted_brake_input * 256.0);
    }
}