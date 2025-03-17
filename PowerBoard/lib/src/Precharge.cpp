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

#define LOG_LEVEL          LOG_DEBUG
#define MAIN_LOOP_PERIOD   1s
#define ERROR_CHECK_PERIOD 100ms

//check these values
#define FLASH_PERIOD       500ms
#define CHARGE_PAUSE    100ms
#define PRECHARGE_CHARGING 2500ms
#define PRECHARGE_OVERLAP  500ms

DigitalIn aux_input(AUX);
DigitalIn cont_12(CONT_12);
AnalogIn rc_voltage(RC_VOLTAGE);
AnalogIn pack_voltage(PACK_VOLTAGE);

//charge
DigitalOut mppt_precharge(MPPT_PRE_EN);
DigitalOut discharge_enable(DIS_CHARGE_EN);

//discharge
DigitalOut motor_precharge(MTR_PRE_EN);
DigitalOut charge_enable(CHARGE_EN);


//initializing can interface
PowerCANInterface vehicle_can_interface(MAIN_CAN_RX, MAIN_CAN_TX, MAIN_CAN_STBY);

Thread precharge_check;
Thread discharge_check;
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
bool cell_voltage_fault = false;
bool has_low_cell_voltage_before = false;
bool has_faulted = false;
int low_cell_voltage_threshold = 27500;
int high_cell_voltage_threshold = 42000;
uint16_t BPS_Cell_Messages = 0;

void start_precharge() {
    //check conditions
    charge_enable.write(0);
    mppt_precharge.write(1);

}

void start_discharge() {
    discharge_enable.write(0);
    motor_precharge.write(1);
}

void battery_precharge(){
    //precharge for battery & mppt
    while (true) {
        //check for faulting
        if (has_faulted || cell_voltage_fault) {
            charge_enable.write(0);
            mppt_precharge.write(0);
            ThisThread::sleep_for(CHARGE_PAUSE);
            continue;
        }

        //checking conditions
        if (charge_relay_status && cont_12.read() && allow_precharge) {
            has_prechaged_before = true;
            allow_precharge = false;
            log_debug("Starting MPPT Precharge");
            start_precharge();

            //let RC reach 05% of pack voltage
            while ((rc_voltage.read()) < 0.95 * (pack_voltage.read())) {
                ThisThread::sleep_for(CHARGE_PAUSE);
            }

            charge_enable.write(1);
            ThisThread::sleep_for(PRECHARGE_OVERLAP);
            mppt_precharge.write(0);
        }

        ThisThread::sleep_for(CHARGE_PAUSE);
    }
    
}

void battery_discharge() {
    while (true) {
        //check for faulting
        if (has_faulted || cell_voltage_fault) {
            discharge_enable.write(0);
            motor_precharge.write(0);
            ThisThread::sleep_for(CHARGE_PAUSE);
            continue;
        }
        
        //checking conditions
        if (discharge_relay_status && cont_12.read() && allow_discharge) {
            has_precharged_discharge_before = true;
            allow_discharge = false;
            log_debug("Starting Motor Precharge");
            start_discharge();

            //wait for RC to reach 95% of pack voltage
            while ((rc_voltage.read()) < 0.95 * (pack_voltage.read())) {
                ThisThread::sleep_for(CHARGE_PAUSE);
            }

            discharge_enable.write(1);
            ThisThread::sleep_for(PRECHARGE_OVERLAP);
            motor_precharge.write(0);
        }

        ThisThread::sleep_for(CHARGE_PAUSE);
    }
}




//NOTE: Contactor - just a gpio pin


int main(){
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");

    precharge_check.start(battery_precharge);
    discharge_check.start(battery_discharge);
    

    while(true){
        //allowing for precharge reset
        if (!cont_12.read() && has_prechaged_before) {
            allow_precharge = true;
            has_prechaged_before = false;
        }
        
        if (!cont_12.read() && has_precharged_discharge_before) {
            allow_discharge = true;
            has_precharged_discharge_before = false;
        }

        //for fault checking 
        if(has_faulted == false){
            has_faulted = contact12V_has_gone_high && !(cont_12.read());
        }
        else if(has_faulted == true || cell_voltage_fault == true){
            mppt_precharge.write(0);
            discharge_enable.write(0);
            motor_precharge.write(0);
            charge_enable.write(0);
        }
        ThisThread::sleep_for(MAIN_LOOP_PERIOD);
    }
    
    

}