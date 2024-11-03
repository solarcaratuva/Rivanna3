#include "TelemetryCANInterface.h"
#include "MotorControllerCANStructs.h"
#include "log.h"

// Constructor for TelemetryCANInterface.h that extends from CANInterface.h in Common include
TelemetryCANInterface::TelemetryCANInterface(PinName rd, PinName td, PinName standby_pin)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000); // Tell can controller to communicate at 250000 bits per second
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
            if (message.id == BPSError_MESSAGE_ID) { //deserializes messages and handles them
                BPSError can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerPowerStatus_AUX_BUS_MESSAGE_ID) {
                MotorControllerPowerStatus can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if(message.id == ECUPowerAuxCommands_MESSAGE_ID) {
                ECUPowerAuxCommands can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            }
        }
    }
}
