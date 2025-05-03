
#ifndef heartbeat_CAN_Struct
#define heartbeat_CAN_Struct

#include "CANStruct.h"
#include "dbc/structs/rivanna3.h"
#include "log.h"

typedef struct Heartbeat : CANStruct, rivanna3_heartbeat_t {
    void serialize(CANMessage *message) {
        rivanna3_heartbeat_pack(message->data, this,
            RIVANNA3_HEARTBEAT_LENGTH);
        message->len = RIVANNA3_HEARTBEAT_LENGTH;
    }

    void deserialize(CANMessage *message) {
        rivanna3_heartbeat_unpack(this, message->data,
            RIVANNA3_HEARTBEAT_LENGTH);
    }

    uint32_t get_message_ID() { return RIVANNA3_HEARTBEAT_FRAME_ID; }

    void log(int level) {
        log_at_level(
            level,
            "Heartbeat: FromWheelBoard %u, FromPowerBoard %u, FromTelemetryBoard %u",
            from_wheel_board, from_power_board, from_telemetry_board);
    }

    size_t format(char* buf, size_t buf_sz) const {
        return std::snprintf(buf, buf_sz,
            "Beat: whl=%u, pwr=%u, tel=%u\n",
            from_wheel_board, from_power_board, from_telemetry_board
        );
    }
    
} Heartbeat;

#endif
