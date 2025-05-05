#include "TelemetryCANInterface.h"
#include "MotorControllerCANStructs.h"
#include "HeartBeatCANStruct.h"
#include "MotorCommandsCANStruct.h"
#include "log.h"
#include "pindef.h"
#include "FATFileSystem.h"
#include "SDBlockDevice.h"
#include "CANStruct.h"

#define LOG_LEVEL LOG_DEBUG

BufferedSerial xbee(RADIO_TX, RADIO_RX, 9600);
// BufferedSerial pc(USB_TX, USB_RX, 115200);
SDBlockDevice sd(SPI2_MOSI, SPI2_MISO, SPI2_SCK, SD_SELECT);
FATFileSystem fs("sd");

TelemetryCANInterface::TelemetryCANInterface(PinName rd, PinName td,
                                     PinName standby_pin)
    : CANInterface(rd, td, standby_pin) {
    can.frequency(250000);

    // sd.init();
    // if (fs.mount(&sd) != 0) {
    //     log_error("SD Card: mount failed");
    // }

    // static const char *LOG_FILE = "/sd/log.txt";
    // FILE *f_init = fopen(LOG_FILE, "a");
    // if (f_init) {
    //     fclose(f_init);
    // }
    // strncpy(_logFilename, LOG_FILE, sizeof(_logFilename));
}

int TelemetryCANInterface::send_message(CANMessage *message) {
    log_debug("Sending CAN message with ID: %d", message->id);
    // send_to_sd(message, message->id);
    return 0;
}


void TelemetryCANInterface::send_to_sd(CANMessage *message, uint16_t message_id) {
    log_debug("SD Card: Sent message with ID: %d", message_id);
    char buf[128];
    size_t len = 0;

    switch (message_id) {
        case BPSPackInformation_MESSAGE_ID: {
            BPSPackInformation s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case BPSError_MESSAGE_ID: {
            BPSError s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case BPSCellVoltage_MESSAGE_ID: {
            BPSCellVoltage s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case BPSCellTemperature_MESSAGE_ID: {
            BPSCellTemperature s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case RIVANNA3_DASHBOARD_COMMANDS_FRAME_ID: {
            DashboardCommands s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case ECUMotorCommands_MESSAGE_ID: {
            ECUMotorCommands s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case ECUPowerAuxCommands_MESSAGE_ID: {
            ECUPowerAuxCommands s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case RIVANNA3_HEARTBEAT_FRAME_ID: {
            Heartbeat s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case RIVANNA3_MOTOR_COMMANDS_FRAME_ID: {
            MotorCommands s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case MotorControllerFrameRequest_AUX_BUS_MESSAGE_ID: {
            MotorControllerFrameRequest s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case MotorControllerPowerStatus_MESSAGE_ID: {
            MotorControllerPowerStatus s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case MotorControllerDriveStatus_MESSAGE_ID: {
            MotorControllerDriveStatus s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case MotorControllerError_MESSAGE_ID: {
            MotorControllerError s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        case PowerAuxError_MESSAGE_ID: {
            PowerAuxError s;
            s.deserialize(message);
            len = s.format(buf, sizeof(buf));
            break;
        }
        default: 
            // unknown ID, skip
            return;
    }

    // Append the formatted line to the log file
    FILE *fp = fopen(_logFilename, "a");
    if (!fp) {
        log_error("SD Card: Failed to open %s", _logFilename);
        return;
    }
    fwrite(buf, 1, len, fp);
    if (len > 0 && buf[len - 1] != '\n') {
        fputc('\n', fp);
    }
    fclose(fp);
}


void TelemetryCANInterface::message_handler() {
    log_set_level(LOG_LEVEL);
    xbee.set_format(8, BufferedSerial::None, 1);
    char *message = "got here";
    xbee.write(message, strlen(message));
    while (true) {
        ThisThread::flags_wait_all(0x1);
        CANMessage msg;
        while (can.read(msg)) {
            // Always log to SD
            // send_to_sd(&msg, msg.id);
            char buf[128];
            size_t len = 0;

            switch (msg.id) {
                case BPSPackInformation_MESSAGE_ID: {
                    BPSPackInformation s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case BPSError_MESSAGE_ID: {
                    BPSError s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case BPSCellVoltage_MESSAGE_ID: {
                    BPSCellVoltage s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case BPSCellTemperature_MESSAGE_ID: {
                    BPSCellTemperature s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case RIVANNA3_DASHBOARD_COMMANDS_FRAME_ID: {
                    DashboardCommands s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case ECUMotorCommands_MESSAGE_ID: {
                    ECUMotorCommands s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case ECUPowerAuxCommands_MESSAGE_ID: {
                    ECUPowerAuxCommands s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case RIVANNA3_HEARTBEAT_FRAME_ID: {
                    Heartbeat s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case RIVANNA3_MOTOR_COMMANDS_FRAME_ID: {
                    MotorCommands s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case MotorControllerFrameRequest_AUX_BUS_MESSAGE_ID: {
                    MotorControllerFrameRequest s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case MotorControllerPowerStatus_MESSAGE_ID: {
                    MotorControllerPowerStatus s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case MotorControllerDriveStatus_MESSAGE_ID: {
                    MotorControllerDriveStatus s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case MotorControllerError_MESSAGE_ID: {
                    MotorControllerError s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                case PowerAuxError_MESSAGE_ID: {
                    PowerAuxError s;
                    s.deserialize(&msg);
                    len = s.format(buf, sizeof(buf));
                    break;
                }
                default:
                    // unknown ID, skip
                    continue;
            }

            // send to radio
            if (len > 0) {
                xbee.write(buf, len);
            }
            else {
                char *zero_length = "length is 0";
                xbee.write(zero_length, strlen(zero_length));
            }
        }
    }
}