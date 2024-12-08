#include "CANStruct.h"
#include "CANInterface.h"

#ifdef NUCLEO_F413ZH 

#include "mbed.h"

// BufferedSerial object

CANInterface::CANInterface(PinName rd, PinName td, PinName standby_pin)
    : serial(PD_1, PD_0, 9600) {
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


// LED for feedback (optional)
DigitalOut led(LED1);

int main() {
    char buffer[128];

    // Configure serial
    serial.set_baud(9600);
    serial.set_format(8, BufferedSerial::None, 1);

    while (true) {
        // Check for received data
        if (serial.readable()) {
            int bytes_read = serial.read(buffer, sizeof(buffer) - 1);
            buffer[bytes_read] = '\0'; // Null-terminate the string
            printf("Received from Pi: %s\n", buffer);

            // Respond back with acknowledgment
            // const char *ack = "Received\n";
            // serial.write(ack, strlen(ack));

            // Toggle LED for feedback
            // led = !led;
        }

        // Send periodic message to Raspberry Pi
        const char *message = "Hello from Mbed!\n";
        serial.write(message, strlen(message));
        // ThisThread::sleep_for(3000ms);
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
