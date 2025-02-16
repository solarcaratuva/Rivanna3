#include "mbed.h"
#include <string.h>
using namespace std::chrono;

BufferedSerial xbee(PA_9, PA_10, 9600);  // XBee radio interface: TX, RX, baud rate 9600
BufferedSerial pc(USBTX, USBRX, 115200);  // Debug console over USB

int main() {
    // Configure both serial ports to use 8 data bits, no parity, 1 stop bit.
    xbee.set_format(8, BufferedSerial::None, 1);
    pc.set_format(8, BufferedSerial::None, 1);fadfadfiajdf;ljk

    char buffer[128] = {0};

    while (true) {
        const char *msg = "XBee Test Message\r\n";
        // Send test message to XBee
        xbee.write(msg, strlen(msg));
        // Also print message on the debug console
        pc.write(msg, strlen(msg));

        // Wait up to 500ms for a response from XBee
        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < 500ms) {
            if (xbee.readable()) {
                int n = xbee.read(buffer, sizeof(buffer));
                if (n > 0) {
                    // Echo any received data to the debug console
                    pc.write(buffer, n);
                }
            }
        }
        // Wait 1 second before sending the next test message
        ThisThread::sleep_for(1000ms);
    }
}