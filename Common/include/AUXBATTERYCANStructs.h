#ifndef aux_battery_status_CAN_Struct
#define aux_battery_status_CAN_Struct

#include "CANStruct.h"
#include "dbc/structs/rivanna3.h"
#include "log.h"

typedef struct AuxBatteryStatus : CANStruct, rivanna3_aux_battery_status_t {
    
    void serialize(CANMessage *message) {
        rivanna3_aux_battery_status_pack(message->data, this,
            RIVANNA3_AUX_BATTERY_STATUS_LENGTH);
        message->len = RIVANNA3_AUX_BATTERY_STATUS_LENGTH;
    }

    void deserialize(CANMessage *message) {
        rivanna3_aux_battery_status_unpack(this, message->data,
            RIVANNA3_AUX_BATTERY_STATUS_LENGTH);
    }

     
    uint32_t get_message_ID() { return RIVANNA3_AUX_BATTERY_STATUS_FRAME_ID; }

    void log(int level) {
        log_at_level(
            level,
            "AuxBatteryStatus: aux_voltage %u, aux_percent_full %u",
            aux_voltage, percent_full);
    }
} AuxBatteryStatus;

#endif
