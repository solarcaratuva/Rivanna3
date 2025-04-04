#ifndef MAIN_POWERBOARD_H // include guard
#define MAIN_POWERBOARD_H

#include <mbed.h>
#include <rtos.h>

extern AnalogIn throttle_pedal;
extern AnalogIn brake_pedal;

extern DigitalIn aux_input;
extern AnalogIn cont_12;
extern AnalogIn rc_voltage_motor;
extern AnalogIn rc_voltage_battery;
extern double pack_voltage;

// charge
extern DigitalOut mppt_precharge;
extern DigitalOut discharge_enable;

// discharge
extern DigitalOut motor_precharge_output;
extern DigitalOut charge_enable;

#endif
