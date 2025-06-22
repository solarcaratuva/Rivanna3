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
        // BufferedSerial xbee(RADIO_TX, RADIO_RX, 9600);
        // const char *message = "Telemetry CAN Interface is running";
        // xbee.write(message, strlen(message));
        ThisThread::sleep_for(1s);
    }
}

void TelemetryCANInterface::handle(PowerAuxError* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling PowerAuxError message, ID: %d", PowerAuxError_MESSAGE_ID);
    send_to_sd(&message, PowerAuxError_MESSAGE_ID);
    send_to_radio(&message, PowerAuxError_MESSAGE_ID);
}

void TelemetryCANInterface::handle(MotorControllerError* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling MotorControllerError message, ID: %d", MotorControllerError_MESSAGE_ID);
    send_to_sd(&message, MotorControllerError_MESSAGE_ID);
    send_to_radio(&message, MotorControllerError_MESSAGE_ID);
}

void TelemetryCANInterface::handle(BPSError* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling BPSError message, ID: %d", BPSError_MESSAGE_ID);
    send_to_sd(&message, BPSError_MESSAGE_ID);
    send_to_radio(&message, BPSError_MESSAGE_ID);
}

void TelemetryCANInterface::handle(ECUMotorCommands* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling ECUMotorCommands message, ID: %d", ECUMotorCommands_MESSAGE_ID);
    send_to_sd(&message, ECUMotorCommands_MESSAGE_ID);
    send_to_radio(&message, ECUMotorCommands_MESSAGE_ID);
}

void TelemetryCANInterface::handle(MotorControllerDriveStatus* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling MotorControllerDriveStatus message, ID: %d", MotorControllerDriveStatus_MESSAGE_ID);
    send_to_sd(&message, MotorControllerDriveStatus_MESSAGE_ID);
    send_to_radio(&message, MotorControllerDriveStatus_MESSAGE_ID);
}

void TelemetryCANInterface::handle(SolarCurrent* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling SolarCurrent message, ID: %d", SolarCurrent_MESSAGE_ID);
    send_to_sd(&message, SolarCurrent_MESSAGE_ID);
    send_to_radio(&message, SolarCurrent_MESSAGE_ID);
}

void TelemetryCANInterface::handle(SolarTemp* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling SolarTemp message, ID: %d", SolarTemp_MESSAGE_ID);
    send_to_sd(&message, SolarTemp_MESSAGE_ID);
    send_to_radio(&message, SolarTemp_MESSAGE_ID);
}

void TelemetryCANInterface::handle(SolarVoltage* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling SolarVoltage message, ID: %d", SolarVoltage_MESSAGE_ID);
    send_to_sd(&message, SolarVoltage_MESSAGE_ID);
    send_to_radio(&message, SolarVoltage_MESSAGE_ID);
}

void TelemetryCANInterface::handle(SolarPhoto* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling SolarPhoto message, ID: %d", SolarPhoto_MESSAGE_ID);
    send_to_sd(&message, SolarPhoto_MESSAGE_ID);
    send_to_radio(&message, SolarPhoto_MESSAGE_ID);
}

void TelemetryCANInterface::handle(BPSPackInformation* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling BPSPackInformation message, ID: %d", BPSPackInformation_MESSAGE_ID);
    send_to_sd(&message, BPSPackInformation_MESSAGE_ID);
    send_to_radio(&message, BPSPackInformation_MESSAGE_ID);
}

void TelemetryCANInterface::handle(MotorControllerPowerStatus* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling MotorControllerPowerStatus message, ID: %d", MotorControllerPowerStatus_MESSAGE_ID);
    send_to_sd(&message, MotorControllerPowerStatus_MESSAGE_ID);
    send_to_radio(&message, MotorControllerPowerStatus_MESSAGE_ID);
}

void TelemetryCANInterface::handle(ECUPowerAuxCommands* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling ECUPowerAuxCommands message, ID: %d", ECUPowerAuxCommands_MESSAGE_ID);
    send_to_sd(&message, ECUPowerAuxCommands_MESSAGE_ID);
    send_to_radio(&message, ECUPowerAuxCommands_MESSAGE_ID);
}
