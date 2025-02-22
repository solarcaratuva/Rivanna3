#ifndef READ_PEDALS_H
#define READ_PEDALS_H

#include <mbed.h>
#include <rtos.h>

uint16_t read_throttle();
uint16_t read_brake();

#endif