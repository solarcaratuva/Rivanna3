#include "PowerCANInterface.h"
#include "MotorControllerCANStructs.h"
#include "log.h"
#include "MotorCommandsCANStruct.h"

#include "log.h"
#include "MotorControllerCANInterface.h"
#include "MotorInterface.h"
#include "PowerCANInterface.h"
#include "pindef.h"
#include "main.h"
#include <mbed.h>
#include <rtos.h>
#include "CAN.h"

#define LOG_LEVEL          LOG_DEBUG
#define MAIN_LOOP_PERIOD   1s
#define ERROR_CHECK_PERIOD 100ms

//check these values
#define FLASH_PERIOD       500ms
#define CHARGE_PAUSE    100ms
#define PRECHARGE_CHARGING 2500ms
#define PRECHARGE_OVERLAP  500ms


//initializing can interface
PowerCANInterface vehicle_can_interface(MAIN_CAN_RX, MAIN_CAN_TX, MAIN_CAN_STBY);

//check
bool allow_precharge = true;
bool allow_discharge = true;
bool has_prechaged_before = false;
bool has_precharged_discharge_before = false;
bool contact12V_has_gone_high = false;
std::chrono::steady_clock::time_point last_time_since_r1r2_input;
int charge_relay_status;
int discharge_relay_status;
int bms_strobe;
bool has_low_cell_voltage_before = false;
bool has_faulted = false;
int low_cell_voltage_threshold = 27500;
int high_cell_voltage_threshold = 42000;
uint16_t BPS_Cell_Messages = 0;
CAN precharge_can(MAIN_CAN_RX, MAIN_CAN_TX);

void start_precharge() {
    //check conditions
    charge_enable.write(0);
    mppt_precharge.write(1);

}

void start_discharge() {
    motor_precharge_output.write(1);
}

void battery_precharge(){
    //precharge for battery & mppt
    CANMessage message;
    if (precharge_can.read(message)) {
        if (message.id == BPSPackInformation_MESSAGE_ID) {
            BPSPackInformation can_struct;
            can_struct.deserialize(&message);
            pack_voltage = static_cast<double>(can_struct.pack_voltage) / 100.0;
            //debug statement
            log_debug("Updated pack voltage from CAN: %.2f V", pack_voltage);
        }
    }

    while (true) {
        //check for faulting
        if (has_faulted) {
            charge_enable.write(0);
            mppt_precharge.write(0);
            ThisThread::sleep_for(CHARGE_PAUSE);
            return;
        }

        //checking conditions
        if (charge_relay_status && cont_12.read() && allow_precharge) {
            has_prechaged_before = true;
            allow_precharge = false;
            log_debug("Starting MPPT Precharge");
            start_precharge();

            //let RC reach 95% of pack voltage
            while ((rc_voltage_battery.read()) < 0.95 * pack_voltage) {
                ThisThread::sleep_for(CHARGE_PAUSE);
            }

            charge_enable.write(1);
            ThisThread::sleep_for(PRECHARGE_OVERLAP);
            mppt_precharge.write(0);
        }

        ThisThread::sleep_for(CHARGE_PAUSE);
    }
    
}

void fault_trap(){
    if (has_faulted) {
        discharge_enable.write(0);
        motor_precharge_output.write(0);
        return;
    }
}

void motor_precharge(){
    log_debug("Motor precharge starting");

    while(cont_12 < 0.1 || !discharge_relay_status){
        //check for fault
        fault_trap();
        ThisThread::sleep_for(CHARGE_PAUSE);
    }

    //start motor precharge
    motor_precharge_output.write(1);

    while ((rc_voltage_motor.read()) < 0.95 * pack_voltage) {
        fault_trap();
        ThisThread::sleep_for(CHARGE_PAUSE);
    }

    ThisThread::sleep_for(CHARGE_PAUSE);
    fault_trap();

    //end motor precharge
    motor_precharge_output.write(0);
    log_debug("Finished motor precharge");

    //main loop
    while (true) {
        Fault_trap();
        ThisThread::sleep_for(CHARGE_PAUSE);
    }
}



//NOTE: ADD TO MAIN
//precharge_check.start(battery_precharge)
//discharge_check.start(battery_discharge)

