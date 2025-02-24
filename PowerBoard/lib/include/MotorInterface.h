#ifndef MOTOR_INTERFACE_H
#define MOTOR_INTERFACE_H

#include <mbed.h>

class MotorInterface {

  public:
    MotorInterface(I2C &throttle_regen);
    int sendThrottle(uint16_t throttle);
    int sendRegen(uint16_t regen);

  private:
    I2C &throttle_regen_bus;
};

#endif