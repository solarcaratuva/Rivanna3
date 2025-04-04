#ifndef MAIN_POWERBOARD_H // include guard
#define MAIN_POWERBOARD_H

#include <mbed.h>
#include <rtos.h>

extern AnalogIn throttle_pedal;
extern AnalogIn brake_pedal;
extern DigitalIn aux_input;
extern double pack_voltage;
extern bool has_faulted;

// precharge
extern bool discharge_relay_status;
extern bool charge_relay_status;
extern AnalogIn rc_voltage_motor;
extern AnalogIn rc_voltage_battery;
extern AnalogIn cont_12;
extern DigitalOut motor_precharge_en;
extern DigitalOut discharge_en;
extern DigitalOut mppt_precharge_en;
extern DigitalOut charge_en;

#endif
