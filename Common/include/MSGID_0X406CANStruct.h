
    #ifndef m_s_g_i_d_0_x406_CAN_Struct
    #define m_s_g_i_d_0_x406_CAN_Struct

    #include "CANStruct.h"
    #include "dbc/structs/bps.h"
    #include "log.h"

    typedef struct BPSPackInformation : CANStruct, bps_msgid_0_x406_t {
        void serialize(CANMessage *message) {
            bps_msgid_0_x406_pack(message->data, this,
                6);
            message->len = 6;
        }

        void deserialize(CANMessage *message) {
            bps_msgid_0_x406_unpack(this, message->data,
                6);
        }

        uint32_t get_message_ID() { return 1030; }

        void log(int level) {
            log_at_level(
                level,
                "MSGID_0X406: pack_voltage %u, Pack_Current %u, Pack_SOC %u, discharge_relay_status %u, charge_relay_status %u, Charger_Safety %u, Charge_Power_Signal %u, Balancing_Active %u",
                pack_voltage, pack_current, pack_soc, discharge_relay_status, charge_relay_status, charger_safety, charge_power_signal, balancing_active);
        }
    } MSGID_0X406;

    #endif
    