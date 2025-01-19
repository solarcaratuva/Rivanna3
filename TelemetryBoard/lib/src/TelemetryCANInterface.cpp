#include "TelemetryCANInterface.h"
#include "MotorControllerCANStructs.h"
#include "log.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"
#include "SDFileSystem.h"

// We assume these pin definitions match your hardware
SDBlockDevice sd(PB_5, PB_4, PB_3, PC_8);  // Example: MOSI, MISO, SCLK, CS
FATFileSystem fs("fs");

// Constructor
TelemetryCANInterface::TelemetryCANInterface(PinName rd, PinName td, PinName standby_pin)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000);
}

void init_sd_card() {
    printf("Initializing SD card...\n");
    if (sd.init() != 0) {
        printf("Failed to initialize SD card.\n");
        return;
    }
    printf("sd size: %llu\n",         sd.size());
    printf("sd read size: %llu\n",    sd.get_read_size());
    printf("sd program size: %llu\n", sd.get_program_size());
    printf("sd erase size: %llu\n",   sd.get_erase_size());

    if (fs.mount(&sd) != 0) {
        printf("Failed to mount the filesystem.\n");
        // Try formatting if the card is not mounted properly
        if (fs.reformat(&sd) != 0) {
            printf("Failed to format the filesystem.\n");
            return;
        }
    }

    printf("SD card initialized and mounted.\n");
}

// Overridden send
int TelemetryCANInterface::send(CANStruct *can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    message.id = can_struct->get_message_ID();
    int result = can.write(message);

    char message_data[17];
    CANInterface::write_CAN_message_data_to_buffer(message_data, &message);

    if (result == 1) {
        log_debug("Sent CAN message ID=0x%03X Len=%d Data=0x%s",
                  message.id, message.len, message_data);
    } else {
        log_error("Failed to send CAN message ID=0x%03X Len=%d Data=0x%s",
                  message.id, message.len, message_data);
    }

    return result;
}

// Continuously reads incoming CAN messages whenever the RX IRQ sets the thread flag
void TelemetryCANInterface::message_handler() {
    while (true) {
        // Wait for a CAN RX interrupt signal
        ThisThread::flags_wait_all(0x1);

        CANMessage message;
        while (can.read(message)) {
            char message_data[17];
            CANInterface::write_CAN_message_data_to_buffer(message_data, &message);

            log_debug("Received CAN message ID=0x%03X Len=%d Data=0x%s",
                      message.id, message.len, message_data);

            if (message.id == ECUPowerAuxCommands_MESSAGE_ID) {
                ECUPowerAuxCommands can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == PowerAuxError_MESSAGE_ID) {
                PowerAuxError can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == SolarCurrent_MESSAGE_ID) {
                SolarCurrent can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == SolarTemp_MESSAGE_ID) {
                SolarTemp can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == SolarVoltage_MESSAGE_ID) {
                SolarVoltage can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == SolarPhoto_MESSAGE_ID) {
                SolarPhoto can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerPowerStatus_MESSAGE_ID
                    || message.id == MotorControllerPowerStatus_AUX_BUS_MESSAGE_ID) {
                log_debug("Motor Power Status");
                MotorControllerPowerStatus can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerDriveStatus_MESSAGE_ID
                    || message.id == MotorControllerDriveStatus_AUX_BUS_MESSAGE_ID) {
                log_debug("Motor Drive Status");
                MotorControllerDriveStatus can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerError_MESSAGE_ID
                    || message.id == MotorControllerError_AUX_BUS_MESSAGE_ID) {
                log_debug("Motor Error Status");
                MotorControllerError can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSPackInformation_MESSAGE_ID) {
                BPSPackInformation can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSError_MESSAGE_ID) {
                BPSError can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSCellVoltage_MESSAGE_ID) {
                BPSCellVoltage can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSCellTemperature_MESSAGE_ID) {
                BPSCellTemperature can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            }
        }
    }
}

// Below are all the "handle(...)" methods that log CAN data to the SD card
void TelemetryCANInterface::handle(ECUPowerAuxCommands *can_struct) {
    // Example of writing out each field
    FILE *file = fopen("/fs/can_log.txt", "a");
    if (!file) {
        log_error("Failed to open SD card file for writing.");
        return;
    }

    fprintf(file,
            "ECUPowerAuxCommands - ID=0x%03X hazards=%d brake_lights=%d headlights=%d left_turn=%d right_turn=%d\n",
            ECUPowerAuxCommands_MESSAGE_ID,
            can_struct->hazards,
            can_struct->brake_lights,
            can_struct->headlights,
            can_struct->left_turn_signal,
            can_struct->right_turn_signal);

    fclose(file);
    log_debug("ECUPowerAuxCommands message written to SD card.");
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
    snprintf(buffer, sizeof(buffer), "Photovoltaic: %f", can_struct->photo);
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
    snprintf(buffer, sizeof(buffer),
             "Pack Voltage: %f, Pack Current: %f",
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
