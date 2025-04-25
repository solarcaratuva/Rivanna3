#ifndef Motor_Commands_CAN_Struct
#define Motor_Commands_CAN_Struct

#include "CANStruct.h"
#include "dbc/structs/rivanna3.h"
#include "log.h"

typedef struct MotorCommands : CANStruct, rivanna3_motor_commands_t {
    void serialize(CANMessage *message) {
        rivanna3_motor_commands_pack(message->data, this,
                                         RIVANNA3_MOTOR_COMMANDS_LENGTH);
        message->len = RIVANNA3_MOTOR_COMMANDS_LENGTH;
    }

    void deserialize(CANMessage *message) {
        rivanna3_motor_commands_unpack(this, message->data,
                                           RIVANNA3_MOTOR_COMMANDS_LENGTH);
    }

    uint32_t get_message_ID() { return RIVANNA3_MOTOR_COMMANDS_FRAME_ID; }

    void log(int level) {
        log_at_level(
            level,
            "MotorCommands: braking %u, regen_drive %u, manual_drive %u, "
            "cruise_drive %u, brake_pedal %u, throttle %u, cruise_speed %u, "
            "regen_braking %u, throttle_pedal %u",
            braking, regen_drive, manual_drive, cruise_drive, brake_pedal,
            throttle, cruise_speed, regen_braking, throttle_pedal);
    }

    std::string toString() const {
        static constexpr const char* FORMAT =
        "MotorCommands: braking %u, regen_drive %u, manual_drive %u, "
        "cruise_drive %u, brake_pedal %u, throttle %u, cruise_speed %u, "
        "regen_braking %u, throttle_pedal %u";
        char buf[256];
        int len = std::snprintf(
            buf, sizeof(buf),
            FORMAT,
            braking, regen_drive, manual_drive,
            cruise_drive, brake_pedal, throttle,
            cruise_speed, regen_braking, throttle_pedal
        );
        return std::string(buf, (len > 0 && len < int(sizeof(buf))) ? len : sizeof(buf)-1);
    }

} MotorCommands;

#endif