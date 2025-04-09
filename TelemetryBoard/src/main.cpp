#include "mbed.h"
#include <string.h>
#include "pindef.h"
#include "EEPROMDriver.h"
#include "log.h"
#include "TelemetryCANInterface.h"

#define LOG_LEVEL          LOG_DEBUG
#define EEPROM_START_ADDR  0x0100   // EEPROM start address for message storage
#define MESSAGE_SIZE       64       // Max size of stored message

// Debug console over USB (ST-Link)
BufferedSerial pc(USB_TX, USB_RX, 115200);

// EEPROM interface (1MHz clock speed)
EEPROMDriver eeprom(SPI2_MOSI, SPI2_MISO, SPI2_SCK, EEPROM_SELECT, 1000000);

bool read_eeprom_message(char *buffer, size_t size);
void write_eeprom_message(const char *message);

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("System startup...");

    char message[MESSAGE_SIZE] = "Time to test EEPROM!";
    write_eeprom_message(message);
    // Attempt to read a valid message from EEPROM
    bool validMessageFound = read_eeprom_message(message, MESSAGE_SIZE);

    // If nothing valid is stored, fall back to a default
    if (!validMessageFound) {
        strcpy(message, "Default EEPROM Message");
        write_eeprom_message(message);
        log_debug("No valid message found. Stored default message in EEPROM.");
    }

    // Main loop: continuously send the EEPROM message over ST-Link
    while (true) {
        log_debug("Sending over ST-Link: %s", message);

        // Send the message to the USB debug console
        pc.write(message, strlen(message));
        pc.write("\r\n", 2);  // Newline for readability

        ThisThread::sleep_for(1s);
    }
}

// Read message from EEPROM
// Returns true if a valid message is found, false otherwise.
bool read_eeprom_message(char *buffer, size_t size) {
    memset(buffer, 0, size);

    // Read stored message length
    uint8_t len = eeprom.read_byte(EEPROM_START_ADDR);
    // Check for invalid length (0xFF indicates an erased or empty EEPROM cell)
    if (len == 0xFF || len == 0 || len > size - 1) {
        log_debug("EEPROM: No valid message found.");
        return false;
    }

    // Read each character
    for (size_t i = 0; i < len; i++) {
        buffer[i] = eeprom.read_byte(EEPROM_START_ADDR + 1 + i);
    }
    buffer[len] = '\0';  // Null-terminate
    pc.write("EEPROM: Read message: ", 22);
    pc.write(buffer, len);
    pc.write("\r\n", 2);

    return true;
}

// Write message to EEPROM
void write_eeprom_message(const char *message) {
    size_t len = strlen(message);

    // Prevent overflow if message is too large
    if (len > MESSAGE_SIZE - 1) {
        len = MESSAGE_SIZE - 1;
    }

    // Store length at the first byte
    eeprom.write_byte(EEPROM_START_ADDR, (uint8_t)len);

    // Store message characters
    for (size_t i = 0; i < len; i++) {
        eeprom.write_byte(EEPROM_START_ADDR + 1 + i, message[i]);
    }
    pc.write("EEPROM: Wrote message: ", 22);
    pc.write(message, len);
    pc.write("\r\n", 2);
    log_debug("EEPROM: Stored message '%s' (size: %d)", message, (int)len);
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
