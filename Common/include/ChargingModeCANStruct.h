
#ifndef charging_mode_CAN_Struct
#define charging_mode_CAN_Struct

#include "CANStruct.h"
#include "dbc/structs/rivanna3.h"
#include "log.h"

typedef struct ChargingMode : CANStruct, rivanna3_charging_mode_t {
    void serialize(CANMessage *message) {
        rivanna3_charging_mode_pack(message->data, this,
            RIVANNA3_CHARGING_MODE_LENGTH);
        message->len = RIVANNA3_CHARGING_MODE_LENGTH;
    }

    void deserialize(CANMessage *message) {
        rivanna3_charging_mode_unpack(this, message->data,
            RIVANNA3_CHARGING_MODE_LENGTH);
    }

    uint32_t get_message_ID() { return RIVANNA3_CHARGING_MODE_FRAME_ID; }

    void log(int level) {
        log_at_level(
            level,
            "ChargingMode: charging_mode_enable %u",
            charging_mode_enable);
    }

    size_t format(char* buf, size_t buf_sz) const {
        return std::snprintf(buf, buf_sz,
            "ChargingMode: charging_mode_enable=%u\n",
            charging_mode_enable
        );
    }

} ChargingMode;

#endif
