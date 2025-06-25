#include <mbed.h>
#include <rtos.h>
#include "main.h"
#include "ReadPedals.h"
#include "log.h"


#define THROTTLE_VOLTAGE_LOW                 0.82 // voltage at 0% throttle
#define THROTTLE_VOLTAGE_HIGH                3.3 // voltage at 100% throttle
#define THROTTLR_VOLTAGE_DIFFERENCE          (THROTTLE_VOLTAGE_HIGH - THROTTLE_VOLTAGE_LOW) // voltage range

#define BRAKE_VOLTAGE_LOW                    1.56 // voltage at 0% brake
#define BRAKE_VOLTAGE_HIGH                   2.66 // voltage at 100% brake
#define BRAKE_VOLTAGE_DIFFERENCE             (BRAKE_VOLTAGE_HIGH - BRAKE_VOLTAGE_LOW) // voltage range



// Reads the throttle pedal value and returns a uint16_t
uint16_t read_throttle() {
    float adjusted_throttle_input = (throttle_pedal.read_voltage() - THROTTLE_VOLTAGE_LOW) / THROTTLR_VOLTAGE_DIFFERENCE;
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
    float adjusted_brake_input = (brake_pedal.read_voltage() - BRAKE_VOLTAGE_LOW) / BRAKE_VOLTAGE_DIFFERENCE;
    if (adjusted_brake_input <= 0.0f) { 
        return 0;
    } else if (adjusted_brake_input >= 1.0f) { 
        return 256;
    } else {
        return (uint16_t)(adjusted_brake_input * 256.0);
    }
}