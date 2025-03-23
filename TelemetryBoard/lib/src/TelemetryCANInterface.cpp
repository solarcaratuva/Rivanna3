#include "TelemetryCANInterface.h"
#include "MotorControllerCANStructs.h"
#include "log.h"
#include "MotorCommandsCANStruct.h"
#include "pindef.h"
#include "FATFileSystem.h"
#include "EEPROMDriver.h"
#include "SDBlockDevice.h"

TelemetryCANInterface::TelemetryCANInterface(PinName rd, PinName td,
                                     PinName standby_pin)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000);
}

int TelemetryCANInterface::send_message(CANMessage *message) {
    log_debug("Sending CAN message with ID: %d", message->id);
    send_to_sd(message, message->id);
    send_to_radio(message, message->id);
    return 0;
}

void TelemetryCANInterface::send_to_sd(CANMessage *message, uint16_t message_id) {
    log_debug("SD Card: Sent message with ID: %d", message_id);
    SDBlockDevice sd(SPI2_MOSI, SPI2_MISO, SPI2_SCK, SD_SELECT);
    FATFileSystem fs("sd");
    int err = sd.init();
    if (err) {
        log_error("SD Card: Error initializing SD card");
        return;
    }
    FILE *fp = fopen("/sd/log.txt", "w");
    char message_data[17];
    CANInterface::write_CAN_message_data_to_buffer(message_data, &message);
    fprintf(fp, "Received message with ID: %d, Data: %s\n", message_id, message_data);
    // Example: Open file, write message->id and message->data, then close file.
}

void TelemetryCANInterface::send_to_radio(CANMessage *message, uint16_t message_id) {

    BufferedSerial xbee(RADIO_TX, RADIO_RX, 9600);
    BufferedSerial pc(USB_TX, USB_RX, 115200);
    xbee.set_format(8, BufferedSerial::None, 1);
    pc.set_format(8, BufferedSerial::None, 1);

    char message_data[17];
    CANInterface::write_CAN_message_data_to_buffer(message_data, &message);
    xbee.write(message_data, std::strlen(message_data));

    auto start = std::chrono::steady_clock::now();
    while (chrono::steady_clock::now() - start < 500ms) {
        if (xbee.readable()) {
            int n = xbee.read(buffer, sizeof(buffer));
            if (n > 0) {
                pc.write(buffer, n);
            }
        }
    }
}

void TelemetryCANInterface::message_handler() {
    while (true) {
        ThisThread::flags_wait_all(0x1);
        CANMessage message;
        if (can.read(&message)) { 
            log_debug("Received CAN message with ID: %d", message.id);
            send_to_sd(&message, message.id);
            send_to_radio(&message, message.id);
        }
        ThisThread::sleep_for(10ms);
    }
}
