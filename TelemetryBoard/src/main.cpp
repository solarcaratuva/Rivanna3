#include "mbed.h"
#include <string.h>
#include "pindef.h" 

using namespace std::chrono;

BufferedSerial xbee(RADIO_TX, RADIO_RX, 9600);
BufferedSerial pc(USB_TX, USB_RX, 115200);        // Debug console over USB

int main() {
    // 8 data bits, no parity, 1 stop bit.
    xbee.set_format(8, BufferedSerial::None, 1);
    pc.set_format(8, BufferedSerial::None, 1);

    const char *msg = "XBee Test Message\r\n";
    char buffer[128] = {0};

    while (true) {
        xbee.write(msg, strlen(msg));
        pc.write(msg, strlen(msg));

        // check for any response from the XBee module and echo to PC console if needed
        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < 500ms) {
            if (xbee.readable()) {
                int n = xbee.read(buffer, sizeof(buffer));
                if (n > 0) {
                    pc.write(buffer, n);
                }
            }
        }
        ThisThread::sleep_for(1s);
    }
}
