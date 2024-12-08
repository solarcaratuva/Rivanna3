#include "CANStruct.h"
#include "CANInterface.h"
#include "mbed.h"


#ifdef TARGET_NUCLEO_F413ZH 

// BufferedSerial object
BufferedSerial serial(PD_1, PD_0, 9600)

CANInterface::CANInterface(PinName rd, PinName td, PinName standby_pin)
    : can(rd, td), standby(standby_pin) {

}

int CANInterface::send(CANStruct *can_struct, CANMessage *message) {
    int result = serial.write(*message, strlen(message));
    return result;
}

bool CANInterface::read(CANMessage *message) {
    ThisThread::flags_wait_all(0x1);
    return serial.read(*message, sizeof(message) - 1);
}

void CANInterface::can_isr() {  }

void CANInterface::write_CAN_message_data_to_buffer(char *buffer,
                                                    CANMessage *message) {
    buffer[0] = '\0';
    for (int i = 0; i < message->len; i += 1) {
        sprintf(buffer + (i * 2), "%02X", message->data[i]);
    }
}

#else

CANInterface::CANInterface(PinName rd, PinName td, PinName standby_pin)
    : can(rd, td), standby(standby_pin) {
    if (standby_pin != NC) {
        standby = 0;
    }
    can_thread.start(callback(this, &CANInterface::message_handler));
    can.attach(callback(this, &CANInterface::can_isr), CAN::RxIrq);
    can.frequency(250000);
}

int CANInterface::send(CANStruct *can_struct, CANMessage *message) {
    can_struct->serialize(message);
    message->id = can_struct->get_message_ID();
    int result = can.write(*message);

    return result;
}

bool CANInterface::read(CANMessage *message) {
    ThisThread::flags_wait_all(0x1);
    return can.read(*message); //can.read accepts a REFFERENCE NOT POINTER
}

void CANInterface::can_isr() { can_thread.flags_set(0x1); }

void CANInterface::write_CAN_message_data_to_buffer(char *buffer,
                                                    CANMessage *message) {
    buffer[0] = '\0';
    for (int i = 0; i < message->len; i += 1) {
        sprintf(buffer + (i * 2), "%02X", message->data[i]);
    }
}




#endif
