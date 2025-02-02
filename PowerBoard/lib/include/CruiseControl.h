#ifndef CRUISE_CONTROL_H
#define CRUISE_CONTROL_H

#include <cstdint>
#include <rtos.h>

constexpr double CRUISE_CONTROL_MAX_SPEED = 40; // TODO fine tune
constexpr double CRUISE_CONTROL_MIN_SPEED = 0;
constexpr double CRUISE_CONTROL_MAX_OUTPUT = 150; // TODO fine tune
constexpr double CRUISE_CONTROL_MIN_OUTPUT = 0;
constexpr double CRUISE_CONTROL_KP = 25; // TODO fine tune
constexpr double CRUISE_CONTROL_KI = 0.1; // TODO fine tune
constexpr double CRUISE_CONTROL_KD = 0; // TODO fine tune
constexpr uint8_t CRUISE_CONTROL_INCREASE_AMOUNT = 5;

class CruiseControl {
public:
uint16_t calculate_cruise_control(double current_speed);
void reset_cruise_control_integral();
void increase_cruise_target();
void decrease_cruise_target();
uint8_t get_cruise_target();
private:
uint64_t prev_time = Kernel::get_ms_count();
double cruise_control_integral = 0;
double cruise_control_previous_error = 0;
uint8_t cruise_control_target = 0;
};

#endif