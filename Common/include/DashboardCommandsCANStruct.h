#ifndef Dashboard_Commands_CAN_Struct
#define Dashboard_Commands_CAN_Struct

#include "CANStruct.h"
#include "dbc/structs/rivanna3.h"
#include "log.h"

typedef struct DashboardCommands : CANStruct, rivanna3_dashboard_commands_t {
    void serialize(CANMessage *message) {
        rivanna3_dashboard_commands_pack(message->data, this,
                                         RIVANNA3_DASHBOARD_COMMANDS_LENGTH);
        message->len = RIVANNA3_DASHBOARD_COMMANDS_LENGTH;
    }

    void deserialize(CANMessage *message) {
        rivanna3_dashboard_commands_unpack(this, message->data,
                                           RIVANNA3_DASHBOARD_COMMANDS_LENGTH);
    }

    uint32_t get_message_ID() { return RIVANNA3_DASHBOARD_COMMANDS_FRAME_ID; }

    void log(int level) {
        log_at_level(
            level,
            "DashboardCommands: hazards %u, left_turn_signal %u, right_turn_signal %u, "
            "regen_en %u, cruise_inc %u, cruise_en %u, cruise_dec %u",
            hazards, left_turn_signal, right_turn_signal, regen_en, cruise_inc,
            cruise_en, cruise_dec);
    }

    std::string toString() const {
        static constexpr const char* FORMAT =
        "DashboardCommands: hazards %u, left_turn_signal %u, right_turn_signal %u, "
        "regen_en %u, cruise_inc %u, cruise_en %u, cruise_dec %u";
        char buf[256];
        int len = std::snprintf(
            buf, sizeof(buf),
            FORMAT,
            hazards, left_turn_signal, right_turn_signal,
            regen_en, cruise_inc, cruise_en, cruise_dec
        );
        return std::string(buf, (len > 0 && len < int(sizeof(buf))) ? len : sizeof(buf)-1);
    }
    
} DashboardCommands;

#endif