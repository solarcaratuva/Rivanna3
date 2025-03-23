#ifndef MAIN_POWERBOARD_H // include guard
#define MAIN_POWERBOARD_H

#include <mbed.h>
#include <rtos.h>

extern AnalogIn throttle_pedal;
extern AnalogIn brake_pedal;

extern DigitalIn aux_input(AUX);
extern AnalogIn cont_12(CONT_12);
extern AnalogIn rc_voltage_motor(RC_VOLTAGE_MOTOR);
extern AnalogIn rc_voltage_battery(RC_VOLTAGE_BATTERY);
extern double pack_voltage;

//charge
extern DigitalOut mppt_precharge(MPPT_PRE_EN);
extern DigitalOut discharge_enable(DIS_CHARGE_EN);

//discharge
extern DigitalOut motor_precharge_output(MTR_PRE_EN);
extern DigitalOut charge_enable(CHARGE_EN);

#endif