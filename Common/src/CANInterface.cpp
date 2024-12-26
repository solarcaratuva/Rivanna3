#include "CANInterface.h"
#include "CANStruct.h"
#include <mbed.h>


#ifdef TARGET_NUCLEO_F413ZH //HIL Testing CAN simulation with BufferedSerial

BufferedSerial serial(PD_1, PD_0, 9600);

CANInterface::CANInterface(PinName rd, PinName td, PinName standby_pin) 
    : can(NC, NC), standby(NC) { 
    //do nothing
}

void CANInterface::CANSetFrequency(int freq) { } //do nothing

int CANInterface::CANWrite(CANMessage message) {
    const void *CAN_message = message.data; 
    size_t CAN_messageLength = message.len;

    if(serial.write(CAN_message, CAN_messageLength) >= 0)
        return 1;
    else
        return -1;
}

//Currently will ignore message.id (since this will only be used between two devices)
//Assumes only data is sent over
int CANInterface::CANRead(CANMessage &message) {
    if (!serial.readable()) {
        return 0;
    }

    char serial_buffer[8]; //max CANMessage size is 8 bytes
    size_t bytes_read = serial.read(serial_buffer, sizeof(serial_buffer));

    message.id = 10; //FILLER
    message.len = bytes_read; 

    //copy data
    for (int i = 0; i < (int) bytes_read; i++) {
        message.data[i] = serial_buffer[i];
    }

    return 1;
}

//keep the same:
void CANInterface::can_isr() { can_thread.flags_set(0x1); } 

//keep the same:
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
}

void CANInterface::CANSetFrequency(int freq) {
    can.frequency(250000);
}

int CANInterface::CANWrite(CANMessage message) {
    return can.write(message);
}

int CANInterface::CANRead(CANMessage  &message) { //ACCEPTS A REFERENCE; NOT A POINTER
    return can.read(message);
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
