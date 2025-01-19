#include "TelemetryCANInterface.h"
#include "MotorControllerCANStructs.h"
#include "log.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

// Constructor for TelemetryCANInterface.h that extends from CANInterface.h in Common include
TelemetryCANInterface::TelemetryCANInterface(PinName rd, PinName td, PinName standby_pin)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000); // Tell can controller to communicate at 250000 bits per second
}

/* Code from ChatGPT */
// Define pins for the microSD card (change according to your hardware)
SDBlockDevice sd(PB_5, PB_4, PB_3, PC_8);  // MOSI, MISO, SCLK, CS (whatever those mean...)
FATFileSystem fs("fs");

uint8_t block[512] = "Hello World!\n"; // Example text to test sd card data transfer

void init_sd_card() {   
    printf("Initializing SD card...\n");
    if (sd.init() != 0) {
        printf("Failed to initialize SD card.\n"); // error statement for SD initialization
        return;
    }
    // Printing details about sd_card, from https://os.mbed.com/docs/mbed-os/v6.16/apis/sdblockdevice.html
    printf("sd size: %llu\n",         sd.size());
    printf("sd read size: %llu\n",    sd.get_read_size());
    printf("sd program size: %llu\n", sd.get_program_size());
    printf("sd erase size: %llu\n",   sd.get_erase_size());

    if (fs.mount(&sd) != 0) {
        printf("Failed to mount the filesystem.\n"); // error statement for failed file system mouting
        // Try formatting if the card is not mounted properly
        if (fs.reformat(&sd) != 0) {
            printf("Failed to format the filesystem.\n"); // error statement for failed reformatting
            return;
        }
    }

    printf("SD card initialized and mounted.\n");
}


int TelemetryCANInterface::send(CANStruct *can_struct) { // Overwriting send method
    CANMessage message;
    // can_struct is a serializable object to be sent over a CAN bus.
    can_struct->serialize(&message); // Formatting or converting message memory address using can_structs serialize function
    message.id = can_struct->get_message_ID(); // assigning can message id
    int result = can.write(message); // CANInterface message is sent, and the result either returns 1 (success) or 0 (fail)

    char message_data[17];

    // Writes data of CAN message to buffer, message_data being the buffer and &message being the converted message
    CANInterface::write_CAN_message_data_to_buffer(message_data, &message);
    if (result == 1) {  
        log_debug("Sent CAN message with ID 0x%03X Length %d Data 0x%s",
                  message.id, message.len, message_data);
                    // If message is sent successfully, logs above message
    } else {
        // this error logging requires changes to mbed-os. make the _can field in CAN.h public instead of private
        // log_error("%d", HAL_FDCAN_GetError(&can._can.CanHandle));
        log_error(
            "Failed to send CAN message with ID 0x%03X Length %d Data 0x%s",
            message.id, message.len, message_data);
        
    }

    return result;
}

void TelemetryCANInterface::message_handler() {
    while (true) {
        ThisThread::flags_wait_all(0x1);
        CANMessage message;
        while (can.read(message)) {
            char message_data[17];

            //TODO: Write to serial message_id, message_data

            // Writes data of CAN message to buffer, message_data being the buffer and &message being the converted message
            CANInterface::write_CAN_message_data_to_buffer(message_data,
                                                           &message);
            log_debug("Received CAN message with ID 0x%03X Length %d Data 0x%s ", message.id, message.len, message_data);
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
            } else if (message.id == MotorControllerPowerStatus_MESSAGE_ID || message.id == MotorControllerPowerStatus_AUX_BUS_MESSAGE_ID) {
                log_debug("Motor Power Status");
                MotorControllerPowerStatus can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerDriveStatus_MESSAGE_ID || message.id == MotorControllerDriveStatus_AUX_BUS_MESSAGE_ID) {
                log_debug("Motor Drive Status");
                MotorControllerDriveStatus can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerError_MESSAGE_ID || message.id == MotorControllerError_AUX_BUS_MESSAGE_ID) {
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