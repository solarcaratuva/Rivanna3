#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>
#include <events/EventQueue.h>

#define LOG_LEVEL                       LOG_DEBUG

DigitalOut pin1(PA_1);

int main() {
    log_set_level(LOG_LEVEL);
    pin1 = !pin1;
    ThisThread::sleep_for(1s);
}