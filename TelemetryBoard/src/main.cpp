#include "TelemetryCANInterface.h"
#include "ThisThread.h"
#include "log.h"
#include <mbed.h>

#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "SDFileSystem.h"


void log_to_sd(CANMessage &message) {
    FILE *fp = fopen("/fs/log.csv", "a");
    if (fp) {
        fprintf(fp, "ID: 0x%03X, Length: %d, Data: ", message.id, message.len);
        for (int i = 0; i < message.len; i++) {
            fprintf(fp, "0x%02X ", message.data[i]);
        }
        fprintf(fp, "\n");
        fclose(fp);
    }
}

void TelemetryCANInterface::handle(ECUPowerAuxCommands *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Headlights: %d, Hazards: %d",
             can_struct->headlights, can_struct->hazards);
    log_to_sd("ECUPowerAuxCommands", buffer);
}

void TelemetryCANInterface::handle(PowerAuxError *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Error Code: %d", can_struct->error_code);
    log_to_sd("PowerAuxError", buffer);
}

void TelemetryCANInterface::handle(SolarCurrent *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Current: %f", can_struct->current);
    log_to_sd("SolarCurrent", buffer);
}

void TelemetryCANInterface::handle(SolarTemp *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Temperature: %f", can_struct->temperature);
    log_to_sd("SolarTemp", buffer);
}

void TelemetryCANInterface::handle(SolarVoltage *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Voltage: %f", can_struct->voltage);
    log_to_sd("SolarVoltage", buffer);
}

void TelemetryCANInterface::handle(SolarPhoto *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Photovoltaic Data: %f", can_struct->photo);
    log_to_sd("SolarPhoto", buffer);
}

void TelemetryCANInterface::handle(MotorControllerPowerStatus *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Power Status: %d", can_struct->power_status);
    log_to_sd("MotorControllerPowerStatus", buffer);
}

void TelemetryCANInterface::handle(MotorControllerDriveStatus *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Drive Status: %d", can_struct->drive_status);
    log_to_sd("MotorControllerDriveStatus", buffer);
}

void TelemetryCANInterface::handle(MotorControllerError *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Error Code: %d", can_struct->error_code);
    log_to_sd("MotorControllerError", buffer);
}

void TelemetryCANInterface::handle(BPSPackInformation *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Pack Voltage: %f, Pack Current: %f",
             can_struct->pack_voltage, can_struct->pack_current);
    log_to_sd("BPSPackInformation", buffer);
}

void TelemetryCANInterface::handle(BPSError *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Error Flags: %d", can_struct->error_flags);
    log_to_sd("BPSError", buffer);
}

void TelemetryCANInterface::handle(BPSCellVoltage *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Cell Voltage: %f", can_struct->cell_voltage);
    log_to_sd("BPSCellVoltage", buffer);
}

void TelemetryCANInterface::handle(BPSCellTemperature *can_struct) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Cell Temperature: %f", can_struct->cell_temperature);
    log_to_sd("BPSCellTemperature", buffer);
}
