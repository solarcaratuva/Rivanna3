#include "MainCANInterface.h"
#include "log.h"
#include <mbed.h>

MainCANInterface::MainCANInterface(PinName rd, PinName td, PinName standby_pin, PinName uart_tx, PinName uart_rx)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000); 
    uartTX = uart_tx;
    uartRX = uart_rx;

}
int MainCANInterface::send_message(CANMessage *message) {
    int result = can.write(*message);

    char message_data[17];
    CANInterface::write_CAN_message_data_to_buffer(message_data, message);
    send_to_pi(message, message->id);
    if (result == 1) {
        log_debug("Sent CAN message with ID 0x%03X Length %d Data 0x%s",
                  message->id, message->len, message_data);
    } else {
        log_error(
            "Failed to send CAN message with ID 0x%03X Length %d Data 0x%s",
            message->id, message->len, message_data);
    }

    return result;
}

int MainCANInterface::send(CANStruct *can_struct) {
    CANMessage message;
    can_struct->serialize(&message);
    message.id = can_struct->get_message_ID();
    int result = can.write(message);

    char message_data[17];
    CANInterface::write_CAN_message_data_to_buffer(message_data, &message);
    //send_to_pi(&message, message.id);
    if (result == 1) {
        log_debug("Sent CAN message with ID 0x%03X Length %d Data 0x%s",
                  message.id, message.len, message_data);
    } else {
        log_error(
            "Failed to send CAN message with ID 0x%03X Length %d Data 0x%s",
            message.id, message.len, message_data);
    }

    return result;
}

void MainCANInterface::message_handler() {
    while (true) {
        ThisThread::flags_wait_all(0x1);
        CANMessage message;
        while (can.read(message)) {
            char message_data[17];
            
            CANInterface::write_CAN_message_data_to_buffer(message_data,
                                                           &message);

            

            log_debug("Received CAN message with ID 0x%03X Length %d Data 0x%s",
                      message.id, message.len, message_data);


            if (message.id == ECUMotorCommands_MESSAGE_ID) {
                send_to_pi(&message, message.id);
                ECUMotorCommands can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == ECUPowerAuxCommands_MESSAGE_ID) {
                send_to_pi(&message, message.id);
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
                send_to_pi(&message, 0x325);
                MotorControllerPowerStatus can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerDriveStatus_MESSAGE_ID || message.id == MotorControllerDriveStatus_AUX_BUS_MESSAGE_ID) {
                log_debug("Motor Drive Status");
                send_to_pi(&message, 0x315);
                MotorControllerDriveStatus can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == MotorControllerError_MESSAGE_ID || message.id == MotorControllerError_AUX_BUS_MESSAGE_ID) {
                log_debug("Motor Error Status");
                send_to_pi(&message, 0x115);
                MotorControllerError can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSPackInformation_MESSAGE_ID) {
                send_to_pi(&message, message.id);
                BPSPackInformation can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSError_MESSAGE_ID) {
                send_to_pi(&message, message.id);
                BPSError can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSCellVoltage_MESSAGE_ID) {
                send_to_pi(&message, message.id);
                BPSCellVoltage can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            } else if (message.id == BPSCellTemperature_MESSAGE_ID) {
                send_to_pi(&message, message.id);
                BPSCellTemperature can_struct;
                can_struct.deserialize(&message);
                handle(&can_struct);
            }
        }
    }
}

void MainCANInterface::send_to_pi(CANMessage *message, uint16_t message_id) {
    if (uartTX != NC) {
        char message_data[17];
        CANInterface::write_CAN_message_data_to_buffer(message_data,
                                                           message);
        log_debug("Sending test message to PI");
        log_debug("Message id is %d", message->id);
        char data_to_pi[25];
        data_to_pi[0] = 249;
        data_to_pi[1] = message->id / 0x0100;; //% 0x1000;
        data_to_pi[2] = message->id % (0x0100);
        for (int i = 0; i < 17; i++) {
            data_to_pi[i+3] = message_data[i];
        }
        data_to_pi[24] = 250;

        log_debug("Raw UART message %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", data_to_pi[0], data_to_pi[1], data_to_pi[2], data_to_pi[3], data_to_pi[4], data_to_pi[5], data_to_pi[6], data_to_pi[7], data_to_pi[8], data_to_pi[9], data_to_pi[10], data_to_pi[11], data_to_pi[12], data_to_pi[13], data_to_pi[14], data_to_pi[15], data_to_pi[16], data_to_pi[17], data_to_pi[18], data_to_pi[19], data_to_pi[20], data_to_pi[21], data_to_pi[22], data_to_pi[23], data_to_pi[24]);
        static BufferedSerial raspberry_pi(uartTX, uartRX, 9600);
        raspberry_pi.write(data_to_pi, sizeof(data_to_pi));
    }
}
