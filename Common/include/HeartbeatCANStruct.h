
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
                FromWheelBoard, FromPowerBoard, FromTelemetryBoard);
        }
    } Heartbeat;

    #endif
    