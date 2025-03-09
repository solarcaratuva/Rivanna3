#ifndef MAIN_POWERBOARD_H // include guard
#define MAIN_POWERBOARD_H

#include <mbed.h>
#include <rtos.h>
#include "MotorInterface.h"

extern AnalogIn throttle_pedal;
extern AnalogIn brake_pedal;

extern MotorInterface motor_interface;

extern bool has_faulted;
extern bool cruise_control_enabled;
extern bool cruise_control_brake_latch;

#endif