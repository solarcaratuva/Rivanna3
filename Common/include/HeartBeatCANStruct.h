
    #ifndef heart_beat_CAN_Struct
    #define heart_beat_CAN_Struct

    #include "CANStruct.h"
    #include "dbc/structs/rivanna3.h"
    #include "log.h"

    typedef struct HeartBeat : CANStruct, rivanna3_heart_beat_t {
        void serialize(CANMessage *message) {
            rivanna3_heart_beat_pack(message->data, this,
                RIVANNA3_HEART_BEAT_LENGTH);
            message->len = RIVANNA3_HEART_BEAT_LENGTH;
        }

        void deserialize(CANMessage *message) {
            rivanna3_heart_beat_unpack(this, message->data,
                RIVANNA3_HEART_BEAT_LENGTH);
        }

        uint32_t get_message_ID() { return RIVANNA3_HEART_BEAT_FRAME_ID; }

        void log(int level) {
            log_at_level(
                level,
                "HeartBeat: FromTelemetryBoard %u, FromWheelBoard %u, FromPowerBoard %u",
                from_telemetry_board, from_wheel_board, from_power_board);
        }
    } HeartBeat;

    #endif
    