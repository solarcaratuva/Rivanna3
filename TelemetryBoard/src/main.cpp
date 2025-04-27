#include "mbed.h"
#include <string.h>
#include "pindef.h"
#include "EEPROMDriver.h"
#include "log.h"
#include "TelemetryCANInterface.h"
#include "ThisThread.h"

#define LOG_LEVEL          LOG_DEBUG
// Thread listening_thread;
TelemetryCANInterface can_interface(MAIN_CAN_RX, MAIN_CAN_TX, MAIN_CAN_STBY);

int main() {
    
    log_set_level(LOG_LEVEL);
    log_debug("System startup...");
    
    while (true) {
         ThisThread::sleep_for(1s);
    }
}