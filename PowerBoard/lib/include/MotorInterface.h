#ifndef MOTOR_INTERFACE_H
#define MOTOR_INTERFACE_H

#include <mbed.h>

#define THROTTLE_ADR    (0b0101111 << 1)
#define REGEN_ADR       (0b0101110 << 1)

class MotorInterface {

  public:
    MotorInterface(I2C &serial_bus);
    int sendThrottle(uint16_t throttle);
    int sendRegen(uint16_t regen);

  private:
    I2C &bus;
};

#endif
