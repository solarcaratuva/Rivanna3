#include "pindef.h"
#include "EEPROMDriver.h"
#include "log.h"
#include "TelemetryCANInterface.h"
#include "ThisThread.h"
#include <mbed.h>
#include <rtos.h>

#define LOG_LEVEL           LOG_DEBUG
#define MAIN_LOOP_PERIOD    100ms


// Thread listening_thread;
TelemetryCANInterface can_interface(MAIN_CAN_RX, MAIN_CAN_TX, MAIN_CAN_STBY);

int main() {
    
    log_set_level(LOG_LEVEL);
    log_debug("System startup...");
    
    while (true) {
        // BufferedSerial xbee(RADIO_TX, RADIO_RX, 9600);
        // const char *message = "Telemetry CAN Interface is running";
        // xbee.write(message, strlen(message));
        ThisThread::sleep_for(MAIN_LOOP_PERIOD);
    }
}

