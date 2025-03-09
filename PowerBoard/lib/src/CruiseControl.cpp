#include "CruiseControl.h"
#include <mbed.h>
#include "MotorInterface.h"
#include "main.h"

uint16_t CruiseControl::calculate_cruise_control(double current_speed){
    uint64_t current_time = Kernel::get_ms_count();
    uint64_t dt = current_time - this->prev_time;
    this->prev_time = current_time;
    if(dt > 20) {
        dt = 10;
    }

    // Calculate error
    double error = this->cruise_control_target - current_speed;
    
    // Proportional term
    double Pout = CRUISE_CONTROL_KP * error;

    // Integral term
    cruise_control_integral += error * dt;
    double Iout = CRUISE_CONTROL_KI * this->cruise_control_integral;

    // Derivative term
    double derivative = (((double) 100.) * (error - this->cruise_control_previous_error)) / ((double) dt);
    double Dout = CRUISE_CONTROL_KD * derivative;
    uint16_t output = (uint16_t)(Pout + Iout + Dout);

    if( output > CRUISE_CONTROL_MAX_OUTPUT ) {
        output = CRUISE_CONTROL_MAX_OUTPUT;
    }
    else if( output < CRUISE_CONTROL_MIN_OUTPUT ) {
        output = CRUISE_CONTROL_MIN_OUTPUT;
    }
    
    this->cruise_control_previous_error = error;
    return output;
}

void CruiseControl::reset_cruise_control_integral() {
    this->cruise_control_integral = 0;
}

void CruiseControl::increase_cruise_target() {
    this->cruise_control_target += CRUISE_CONTROL_INCREASE_AMOUNT;
    if(this->cruise_control_target > CRUISE_CONTROL_MAX_SPEED) {
        this->cruise_control_target = CRUISE_CONTROL_MAX_SPEED;
    }
}

void CruiseControl::decrease_cruise_target() {
    this->cruise_control_target -= CRUISE_CONTROL_INCREASE_AMOUNT;
    if(this->cruise_control_target < CRUISE_CONTROL_MIN_SPEED) {
        this->cruise_control_target = CRUISE_CONTROL_MIN_SPEED;
    }
}

uint8_t CruiseControl::get_cruise_target() {
    return this->cruise_control_target;
}

void CruiseControl::send_cruise_control_to_motor(uint16_t motor_rpm) {
    uint16_t current_speed_mph = (double)motor_rpm * MOTOR_RPM_TO_MPH_RATIO;
    if(!has_faulted && cruise_control_enabled && !cruise_control_brake_latch) {
        uint16_t next_cruise_output = calculate_cruise_control(current_speed_mph);
        motor_interface.sendThrottle(next_cruise_output);
        motor_interface.sendRegen(0);
    } else {
        reset_cruise_control_integral();
    }
}