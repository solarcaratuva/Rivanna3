#include "TelemetryCANInterface.h"
#include "ThisThread.h"
#include "log.h"
#include <mbed.h>
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

#define LOG_LEVEL LOG_ERROR

// Create one TelemetryCANInterface globally
TelemetryCANInterface vehicle_can_interface(CAN_RX, CAN_TX, CAN_STBY);

// Thread that will continuously read CAN messages and log them
Thread canThread;

// Simple wrapper to periodically call message_handler()
void can_processing_task() {
    // NOTE: TelemetryCANInterface::message_handler() has its own `while(true)`,
    // so once we call it, it never returns. An alternative is to remove that
    // inner while(true) and place a loop here. But for now, we can just call:
    vehicle_can_interface.message_handler();
}

void log_to_sd(const char *tag, const char *data) {
    FILE *fp = fopen("/fs/log.csv", "a");
    if (fp) {
        fprintf(fp, "[%s]: %s\n", tag, data);
        fclose(fp);
    } else {
        log_error("Failed to write to SD card.");
    } 
}

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("Starting Telemetry System main()...");

    // 1. Initialize the SD card
    init_sd_card();

    // 2. Log startup message (must pass two strings)
    log_to_sd("System", "initialized");

    // 3. Start a thread to handle CAN messages
    canThread.start(can_processing_task);

    // 4. Main loop can do other tasks if desired
    while (true) {
        ThisThread::sleep_for(1s);
        // Additional telemetry tasks if needed...
    }
}
