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
DigitalIn rc_voltage(RC_VOLTAGE);
DigitalIn pack_voltage(PACK_VOLTAGE);

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

    charge_enable.write(0);
    mppt_precharge.write(1);

}

void start_discharge() {
    discharge_enable.write(0);
    motor_precharge.write(1);
}

void battery_precharge(){
    //precharge for battery & mppt
    while(true){
        int contact12_status = cont_12.read();

        //discharge_relay_status is high & contactor12 rising status --> start precharge
        if(contact12_status == 1 && discharge_relay_status == false){
            discharge_relay_status = readCANMessage("vehicle_can_interface");
            std::precharge_check::sleep_for(std::chrono::milliseconds(100)); 
            log_debug("Precharge has started");

        }
        start_precharge(); 

        //rc voltage reaches .95 of pack coltage --> discharge
        while (rc_voltage < 0.95 * pack_voltage) {
            std::precharge_check::sleep_for(std::chrono::milliseconds(100)); 
        }

        //close contactor & disable motor precharge
        start_discharge();
        std::precharge_check::sleep_for(std::chrono::milliseconds(100)); //small delay
        motor_precharge.write(0);
        log_debug("Precharge complete, discharge enabled.");

        std::precharge_check::sleep_for(std::chrono::milliseconds(100)); //small delay for entire system

    }
}



//NOTE: Contactor - just a gpio pin


int main(){
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");

    precharge_check.start(battery_precharge);

    while(true){
        //for fault checking 
        if(has_faulted == false){
            has_faulted = contact12V_has_gone_high && !(cont_12.read());
        }
        else if(has_faulted == true){
            charge_enable = true;
            mppt_precharge = false;
        }
    }
    

}