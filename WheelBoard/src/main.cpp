#include "mbed.h"

// BufferedSerial object
BufferedSerial serial(PD_1, PD_0, 9600); // Replace with appropriate TX and RX pins

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
            const char *ack = "Message received on Mbed\n";
            serial.write(ack, strlen(ack));

            // Toggle LED for feedback
            led = !led;
        }

        // Send periodic message to Raspberry Pi
        const char *message = "Hello from Mbed!\n";
        serial.write(message, strlen(message));
        ThisThread::sleep_for(2000ms);
    }
}