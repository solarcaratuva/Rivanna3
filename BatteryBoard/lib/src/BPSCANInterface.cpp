#include "BPSCANInterface.h"
#include "log.h"

BPSCANInterface::BPSCANInterface(PinName rd, PinName td, PinName standby_pin)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000);
}


int BPSCANInterface::send(CANStruct *can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    message.id = can_struct->get_message_ID();
    int result = can.write(message);

    char message_data[17];

    CANInterface::write_CAN_message_data_to_buffer(message_data, &message);
    if (result == 1) {  
        log_debug("Sent CAN message with ID 0x%03X Length %d Data 0x%s",
                  message.id, message.len, message_data);
    } else {
        // this error logging requires changes to mbed-os. make the _can field in CAN.h public instead of private
        // log_error("%d", HAL_FDCAN_GetError(&can._can.CanHandle));
        log_error(
            "Failed to send CAN message with ID 0x%03X Length %d Data 0x%s",
            message.id, message.len, message_data);
        
    }

    return result;
}

void BPSCANInterface::message_handler() {
    while (true) {
        ThisThread::flags_wait_all(0x1);
        CANMessage message;
        
        while (can.read(message)) {
            // Forwards all messages to vehicle can bus
            message_forwarder(&message);

            char message_data[17];
            CANInterface::write_CAN_message_data_to_buffer(message_data,
                                                           &message);
            log_debug("Received CAN message with ID 0x%03X Length %d Data 0x%s "
                      "from BPS",
                      message.id, message.len, message_data);

            if (message.id == BPSError_MESSAGE_ID) {
                BPSError can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSPackInformation_MESSAGE_ID) {
                BPSPackInformation can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            }
            else if (message.id == BPSCellVoltage_MESSAGE_ID) {
                BPSCellVoltage can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            }
        }
    }
}
