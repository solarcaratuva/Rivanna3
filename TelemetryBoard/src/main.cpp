#include "mbed.h"
#include <string.h>
#include "pindef.h"
#include "EEPROMDriver.h"
#include "log.h"
#include "TelemetryCANInterface.h"
#include "ThisThread.h"

#define LOG_LEVEL          LOG_DEBUG

BufferedSerial xbee(RADIO_TX, RADIO_RX, 9600);
BufferedSerial pc(USB_TX, USB_RX, 115200);
Thread listening_thread;

// This thread function periodically sends out a “listening for CAN messages” message.
void periodic_listening_message() {
    const char *listening_message = "Listening for CAN messages\n";
    
    while (true) {
        // Send the listening message to both the radio and the PC console
        xbee.write(listening_message, strlen(listening_message));
        pc.write(listening_message, strlen(listening_message));
        
        // Wait 5 seconds before sending again
        ThisThread::sleep_for(5s);
    }
}

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("System startup...");

    listening_thread.start(periodic_listening_message);

    // Configure the serial interfaces.
    xbee.set_format(8, BufferedSerial::None, 1);
    pc.set_format(8, BufferedSerial::None, 1);

    // Main thread can be left idle or used for other application tasks.
    while (true) {
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

void TelemetryCANInterface::handle(BPSCellVoltage* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling BPSCellVoltage message, ID: %d", BPSCellVoltage_MESSAGE_ID);
    send_to_sd(&message, BPSCellVoltage_MESSAGE_ID);
    send_to_radio(&message, BPSCellVoltage_MESSAGE_ID);
}

void TelemetryCANInterface::handle(BPSCellTemperature* can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    log_debug("Handling BPSCellTemperature message, ID: %d", BPSCellTemperature_MESSAGE_ID);
    send_to_sd(&message, BPSCellTemperature_MESSAGE_ID);
    send_to_radio(&message, BPSCellTemperature_MESSAGE_ID);
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
