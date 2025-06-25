
    #ifndef m_s_g_i_d_0_x106_CAN_Struct
    #define m_s_g_i_d_0_x106_CAN_Struct

    #include "CANStruct.h"
    #include "dbc/structs/bps.h"
    #include "log.h"

    typedef struct BPSError : CANStruct, bps_msgid_0_x106_t {
        void serialize(CANMessage *message) {
            bps_msgid_0_x106_pack(message->data, this,
                3);
            message->len = 3;
        }

        void deserialize(CANMessage *message) {
            bps_msgid_0_x106_unpack(this, message->data,
                3);
        }

        uint32_t get_message_ID() { return 262; }

        void log(int level) {
            log_at_level(
                level,
                "MSGID_0X106: DTC_P0A1F_Internal_Cell_Communication_Fault %u, Weak_Cell_Fault %u, Low_Cell_Voltage_Fault %u, Cell_Open_Wiring_Fault %u, Current_Sensor_Fault %u, Weak_Pack_Fault %u, Thermistor_Fault %u, CAN_Communication_Fault %u, Redundant_Power_Supply_Fault %u, High_Voltage_Isolation_Fault %u, ChargeEnable_Relay_Fault %u, DischargeEnable_Relay_Fault %u, Internal_Hardware_Fault %u, DTC_P0A0A_Internal_Heatsink_Thermistor_Fault %u, Internal_Logic_Fault %u, DTC_P0A0C_Highest_Cell_Voltage_Too_High_Fault %u, DTC_P0A0E_Lowest_Cell_Voltage_Too_Low_Fault %u, Pack_Too_Hot_Fault %u",
                dtc_p0_a1_f_internal_cell_communication_fault, weak_cell_fault, low_cell_voltage_fault, cell_open_wiring_fault, current_sensor_fault, weak_pack_fault, thermistor_fault, can_communication_fault, redundant_power_supply_fault, high_voltage_isolation_fault, charge_enable_relay_fault, discharge_enable_relay_fault, internal_hardware_fault, dtc_p0_a0_a_internal_heatsink_thermistor_fault, internal_logic_fault, dtc_p0_a0_c_highest_cell_voltage_too_high_fault, dtc_p0_a0_e_lowest_cell_voltage_too_low_fault, pack_too_hot_fault);
        }
    } MSGID_0X106;

    #endif
    