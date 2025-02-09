#include "CANInterface.h"
#include "CANStruct.h"
#include <mbed.h>


#ifdef TARGET_NUCLEO_F413ZH //HIL Testing CAN simulation with BufferedSerial

BufferedSerial serial(PD_1, PD_0, 9600);

CANInterface::CANInterface(PinName rd, PinName td, PinName standby_pin) 
    : can(rd, td), standby(standby_pin) { 
    //do nothing
}

void CANInterface::CANSetFrequency(int freq) { } //do nothing

int CANInterface::CANWrite(CANMessage message) {
    uint8_t* CAN_messageData = message.data; //message data
    uint8_t CAN_messageLength = message.len; //message length (excluding ID length)
    uint16_t CAN_messageID = message.id;

    char message_buffer[10];
    
    //first 2 bytes is the ID
    message_buffer[0] = (uint8_t) (CAN_messageID >> 8);
    message_buffer[1] = (uint8_t) (CAN_messageID & 0xFF);

    //fill rest of message:
    for(int i = 0; i < (int) CAN_messageLength; i++) {
        message_buffer[i + 2] = CAN_messageData[i];
    }

    message_buffer[CAN_messageLength] = 10; //Raspberry PI needs a newline character 

    if(serial.write(message_buffer, CAN_messageLength + 2) >= 0)
        return 1;
    else
        return -1;
}

//Assumes first 2 bytes is the message ID
int CANInterface::CANRead(CANMessage &message) {
    //hang until it can read
    while(!serial.readable()){
        ThisThread::sleep_for(10ms); //sleep for 10ms
    }

    char serial_buffer[10]; //max fake CANMessage size is 2 byte ID + 8 byte data
    size_t bytes_read = serial.read(serial_buffer, sizeof(serial_buffer));

    //minimum length for a fake CAN message
    if (bytes_read >= 3) {
        message.id = (((int) serial_buffer[0])<<8) | (serial_buffer[1]); 
        message.len = bytes_read - 2; 

        //copy data
        for (int i = 0; i < (int) message.len; i++) {
            message.data[i] = serial_buffer[i + 2];
        }

        return 1;
    }
    else {
        //invalid CAN message recieved
        return 0; 
    }
    
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
    ThisThread::flags_wait_all(0x1);
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
