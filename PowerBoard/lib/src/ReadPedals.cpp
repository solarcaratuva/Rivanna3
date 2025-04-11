#include <mbed.h>
#include <rtos.h>
#include "main.h"
#include "ReadPedals.h"
#include "log.h"

#define THROTTLE_LOW_VOLTAGE         0.66
#define THROTTLE_LOW_VOLTAGE_BUFFER  0.20
#define THROTTLE_HIGH_VOLTAGE        3.08
#define THROTTLE_HIGH_VOLTAGE_BUFFER 0.10

#define VOLTAGE_LOW                  1.56
#define VOLTAGE_HIGH                 2.66
#define VOLTAGE_DIFFERENCE           (VOLTAGE_HIGH - VOLTAGE_LOW)



// Reads the throttle pedal value and returns a uint16_t
uint16_t read_throttle() {
    log_error("Pedal Throttle %d", throttle_pedal.read_voltage());
    float adjusted_throttle_input = (throttle_pedal.read_voltage() - VOLTAGE_LOW) / VOLTAGE_DIFFERENCE;
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