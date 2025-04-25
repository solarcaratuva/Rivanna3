#include "TelemetryCANInterface.h"
#include "MotorControllerCANStructs.h"
#include "log.h"
#include "MotorCommandsCANStruct.h"
#include "pindef.h"
#include "FATFileSystem.h"
#include "EEPROMDriver.h"
#include "SDBlockDevice.h"
#include "dbc/structs/rivanna2.h"
#include "CANStruct.h"

#define LOG_LEVEL LOG_DEBUG

BufferedSerial xbee(RADIO_TX, RADIO_RX, 9600);
BufferedSerial pc(USB_TX, USB_RX, 115200);
SDBlockDevice sd(SPI2_MOSI, SPI2_MISO, SPI2_SCK, SD_SELECT);
FATFileSystem fs("sd");

TelemetryCANInterface::TelemetryCANInterface(PinName rd, PinName td,
                                     PinName standby_pin)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000);
}

int TelemetryCANInterface::send_message(CANMessage *message) {
    log_debug("Sending CAN message with ID: %d", message->id);
    send_to_sd(message, message->id);
    send_to_radio(message, message->id);
    return 0;
}

void TelemetryCANInterface::send_to_sd(CANMessage *message, uint16_t message_id) {
    log_debug("SD Card: Sent message with ID: %d", message_id);
    int err = sd.init();
    if (err) {
        log_error("SD Card: Error initializing SD card");
        return;
    }
    char message_data[17];
    FILE *fp = fopen("/sd/log.txt", "a");
    CANInterface::write_CAN_message_data_to_buffer(message_data, message);
    fprintf(fp, "Received message with ID: %d, Data: %s\n", message_id, message_data);
    fclose(fp);
    // Example: Open file, write message->id and message->data, then close file.
}

void TelemetryCANInterface::send_to_radio(CANMessage *message, uint16_t message_id) {

    xbee.set_format(8, BufferedSerial::None, 1);
    pc.set_format(8, BufferedSerial::None, 1);

    char message_data[17];
    CANInterface::write_CAN_message_data_to_buffer(message_data, message);
    xbee.write(message_data, std::strlen(message_data));

    auto start = std::chrono::steady_clock::now();
    char buffer[128];
    while (chrono::steady_clock::now() - start < 500ms) {
        if (xbee.readable()) {
            int n = xbee.read(buffer, sizeof(buffer));
            if (n > 0) {
                pc.write(buffer, n);
            }
        }
    }
}

void TelemetryCANInterface::message_handler() {
    log_set_level(LOG_LEVEL);
    while (true) {
        ThisThread::flags_wait_all(0x1);
        CANMessage message;
        while (can.read(message)) {
            char message_data[17];

            //TODO: Write to serial message_id, message_data

            CANInterface::write_CAN_message_data_to_buffer(message_data,
                                                           &message);
            log_debug("Received CAN message with ID 0x%03X Length %d Data 0x%s ", message.id, message.len, message_data);
            if (message.id == PowerAuxError_MESSAGE_ID) {
                PowerAuxError can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == MotorControllerError_MESSAGE_ID) {
                MotorControllerError can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == BPSError_MESSAGE_ID) {
                BPSError can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            } 
            else if (message.id == ECUMotorCommands_MESSAGE_ID) {
                ECUMotorCommands can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == MotorControllerDriveStatus_MESSAGE_ID) {
                MotorControllerDriveStatus can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == SolarCurrent_MESSAGE_ID) {
                SolarCurrent can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == SolarTemp_MESSAGE_ID) {
                SolarTemp can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == SolarVoltage_MESSAGE_ID) {
                SolarVoltage can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == SolarPhoto_MESSAGE_ID) {
                SolarPhoto can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == BPSPackInformation_MESSAGE_ID) {
                BPSPackInformation can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == BPSCellVoltage_MESSAGE_ID) {
                BPSCellVoltage can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == BPSCellTemperature_MESSAGE_ID) {
                BPSCellTemperature can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if (message.id == MotorControllerPowerStatus_MESSAGE_ID) {
                MotorControllerPowerStatus can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if(message.id == ECUPowerAuxCommands_MESSAGE_ID) {
                ECUPowerAuxCommands can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else if(message.id == 512) {
                MotorCommands can_struct;
                can_struct.deserialize(&message);
                send_to_sd(&message, message.id);
                send_to_radio(&message, message.id);
                can_struct.log(LOG_LEVEL);
            }
            else {
                std::string msg = "Unknown CAN message ID: " + std::to_string(message.id) + "\n";
                xbee.write(msg.c_str(), msg.length());
                pc.write(msg.c_str(), msg.length());
                // log_debug("Unknown CAN message ID: %d", message.id);
            }
        }
    }
}

// void TelemetryCANInterface::message_handler() {
//     while (true) {
//         ThisThread::flags_wait_all(0x1);
//         CANMessage message;
//         while (can.read(message)) {
//             char message_data[17];
//             CANInterface::write_CAN_message_data_to_buffer(message_data, &message);
//             log_debug(
//                 "Received CAN message with ID 0x%03X Length %d Data 0x%s ",
//                 message.id, message.len, message_data);
                
//             send_to_radio(&message, message.id);
//         }
//     }
// }