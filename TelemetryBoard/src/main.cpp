#include "TelemetryCANInterface.h"
#include "ThisThread.h"
#include "log.h"
#include <mbed.h>

#define LOG_LEVEL          LOG_ERROR

TelemetryCANInterface vehicle_can_interface(CAN_RX, CAN_TX, CAN_STBY);
Thread sdCardThread;

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");
    
    motor_thread.start(motor_message_handler);
    signalFlashThread.start(signalFlashHandler);

    drl = PIN_ON;

    while (true) {
        log_debug("Main thread loop");

        read_inputs();

        ThisThread::sleep_for(MAIN_LOOP_PERIOD);

        //  hazards, brake_lights, headlights, left_turn_signal,
        //              right_turn_signal
        power_aux_out.hazards = flashHazards;
        power_aux_out.brake_lights = brakeLightsSwitch;
        power_aux_out.headlights = 0;
        power_aux_out.left_turn_signal = flashLSignal;
        power_aux_out.right_turn_signal = flashRSignal;

        vehicle_can_interface.send(&power_aux_out);
    }
}

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("Starting Telemetry System main()...");

    init_sd_card();

    



    // 1. Initialize the SD card
    

    // 2. Set up the CAN interface
    TelemetryCANInterface telemetry_can(CAN_RX_PIN, CAN_TX_PIN, CAN_STBY_PIN);

    // 3. Log startup message
    log_to_sd("System initialized.\n");

    // 4. Main loop for processing messages
    while (true) {
        ThisThread::sleep_for(1s);
    }
}


void TelemetryCANInterface::handle(ECUPowerAuxCommands *can_struct) {
    FILE *file = fopen("/fs/can_log.txt", "a"); // Open file in append mode
        if (file == NULL) {
            log_error("Failed to open SD card file for writing.");
            return;
        }

        // Log the message details
        fprintf(file, "ECUPowerAuxCommands - ID: 0x%03X, Data: 0x%08X\n", 
                ECUPowerAuxCommands_MESSAGE_ID, can_struct->);

        fclose(file); // Close the file
        log_debug("ECUPowerAuxCommands message written to SD card.");

}

void TelemetryCANInterface::handle(PowerAuxError *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarCurrent *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarTemp *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarVoltage *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarPhoto *can_struct) {
    
}

void TelemetryCANInterface::handle(MotorControllerPowerStatus *can_struct) {
    
}

void TelemetryCANInterface::handle(MotorControllerDriveStatus *can_struct) {
    
}

void TelemetryCANInterface::handle(MotorControllerError *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSPackInformation *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSError *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSCellVoltage *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSCellTemperature *can_struct) {
    
}