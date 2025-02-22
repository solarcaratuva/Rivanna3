#include <mbed.h>
#include <rtos.h>
#include "main.h"
#include "ReadPedals.h"

#define THROTTLE_LOW_VOLTAGE         0.66
#define THROTTLE_LOW_VOLTAGE_BUFFER  0.20
#define THROTTLE_HIGH_VOLTAGE        3.08
#define THROTTLE_HIGH_VOLTAGE_BUFFER 0.10



// Reads the throttle pedal value and returns a uint16_t
uint16_t read_throttle() {
    float adjusted_throttle_input =
        ((throttle_pedal.read_voltage() - THROTTLE_LOW_VOLTAGE -
          THROTTLE_LOW_VOLTAGE_BUFFER) /
         (THROTTLE_HIGH_VOLTAGE - THROTTLE_HIGH_VOLTAGE_BUFFER -
          THROTTLE_LOW_VOLTAGE - THROTTLE_LOW_VOLTAGE_BUFFER));
    if (adjusted_throttle_input <= 0.0f) {
        return 0;
    } else if (adjusted_throttle_input >= 1.0f) {
        return 256;
    } else {
        return (uint16_t)(adjusted_throttle_input * 256.0);
    }
}

// TODO Reads the brake pedal value and returns a uint16_t
uint16_t read_brake() {
    float adjusted_brake_input =
        ((brake_pedal.read_voltage() - THROTTLE_LOW_VOLTAGE -
          THROTTLE_LOW_VOLTAGE_BUFFER) /
         (THROTTLE_HIGH_VOLTAGE - THROTTLE_HIGH_VOLTAGE_BUFFER -
          THROTTLE_LOW_VOLTAGE - THROTTLE_LOW_VOLTAGE_BUFFER));
    if (adjusted_brake_input <= 0.0f) {
        return 0;
    } else if (adjusted_brake_input >= 1.0f) {
        return 256;
    } else {
        return (uint16_t)(adjusted_brake_input * 256.0);
    }
}