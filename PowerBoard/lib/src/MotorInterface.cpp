#include "MotorInterface.h"
#include "pindef.h"

/**
 * Motor driver that controls the throttle and regen values
 * @param throttle Mbed I2C object for throttle communication
 * @param regen Mbed I2C object for regen communication
 */
MotorInterface::MotorInterface(I2C &throttle, I2C &regen)
    : throttleBus(throttle), regenBus(regen) {}

/**
 * Sends throttle value to digital POT
 * @param throttle Throttle value [0, 255]
 */
int MotorInterface::sendThrottle(uint16_t throttle) {
    uint16_t updated_throttle = 0x100 - throttle;
    char cmd[2];
    cmd[0] = (updated_throttle & 0x100) >> 8;
    cmd[1] = updated_throttle & 0xFF;
    int result = throttleBus.write(0x5C, cmd, 2);
    return result;
}

/**
 * Sends regen value to digital POT
 * @param regen Regen value [0, 255]
 */
int MotorInterface::sendRegen(uint16_t regen) {
    uint16_t updated_regen = 0x100 - regen;
    char cmd[2];
    cmd[0] = (updated_regen & 0x100) >> 8;
    cmd[1] = updated_regen & 0xFF;
    int result = regenBus.write(0x5C, cmd, 2);
    return result;
}
