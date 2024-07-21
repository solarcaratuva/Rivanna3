#include "BPSCANInterface.h"
#include "BatteryBoardCANInterface.h"
#include "ECUCANStructs.h"

#include "DigitalOut.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>

#define LOG_LEVEL          LOG_DEBUG
#define MAIN_LOOP_PERIOD   1s
#define ERROR_CHECK_PERIOD 100ms
#define FLASH_PERIOD       500ms
#define CHARGE_PAUSE    100ms
#define PRECHARGE_CHARGING 2500ms
#define PRECHARGE_OVERLAP  500ms

DigitalIn aux_input(AUX_PLUS);
DigitalIn dcdc_input(DCDC_PLUS);
DigitalIn fantech_input(FanTech);
DigitalIn contact12_input(CONTACT_12);

/* TODO: verify (/update) the information in these comments and then update the two methods starting at line 63
Everything needs to be precharged: Both discharge and charge (they are current in opposite directions)
MPPT precharge must be enabled before the motor can be discharged
Motor precharge must be enabled before the motor can be charged
Precharge must happen when the car is turned on and if the discharge/charge have been disabled for some time (30s)
*/

// Charge
DigitalOut mppt_precharge(MPPT_PRECHARGE);
DigitalOut discharge_enable(DISCHARGE_ENABLE);

// Discharge
DigitalOut motor_precharge(MOTOR_PRECHARGE);
DigitalOut charge_enable(CHARGE_ENABLE);


// vehicle_can_interface is the CAN line between all the boards
// bps_can_interface is the CAN line to the BPS
// initializing a can_interface starts a thread that listens for CAN messages

BatteryBoardCANInterface vehicle_can_interface(CAN_RX, CAN_TX, CAN_STBY);
BPSCANInterface bps_can_interface(BMS_CAN1_RX, BMS_CAN1_TX, BMS_CAN1_STBY);

Thread precharge_check;
Thread discharge_check;
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

//  ------------------------------------------------------
// TODO: make sure these methods are correct
// Outputs
// Four pins that are used in this
// mppt_precharge = PB_1
// motor_precharge = PA_7
// charge_enable = PA_5
// discharge_enable = PA_6

// Enables switch to start precharging
void start_precharge() {

    charge_enable.write(0);
    mppt_precharge.write(1);

}

void start_discharge() {
    discharge_enable.write(0);
    motor_precharge.write(1);
}
// ------------------------------------------------------


void battery_precharge() {
    while (true) {
        int contact_status = contact12_input.read();

        // Start precharge if state allows precharge + high voltage at contact12 + charge_relay_status high from BMS


        //charge_relay_status = 1;    
        if(has_faulted || cell_voltage_fault) {
            charge_enable.write(0);
            mppt_precharge.write(0);
            //allow_precharge = true;
            continue;
        }    
        else if(charge_relay_status && contact_status && allow_precharge && !cell_voltage_fault && !has_faulted) {
            has_prechaged_before = true;
            log_debug("Is Precharging Charge");
            allow_precharge = false; // after precharge starts don't restart it
            start_precharge();
            ThisThread::sleep_for(PRECHARGE_CHARGING);
            charge_enable.write(1);
            ThisThread::sleep_for(PRECHARGE_OVERLAP);
            mppt_precharge.write(0);
            continue;
        }
        /*
        if((!charge_relay_status || !contact_status) && has_prechaged_before && !allow_precharge) {
            // wait 30 seconds for charge_relay_status from the BMS to go low OR
            // wait 30 seconds for contact12 to go low

            bool dont_allow_charge = false;
            chrono::steady_clock::time_point start = chrono::steady_clock::now();
            while(chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() < 30) {
                 // TODO check if this should be a && or an ||
                 // if its && that means for precharge to start again only one of the two conditions has to be low for 30s
                if(charge_relay_status && contact_status) {
                    dont_allow_charge = true;
                    break;
                }
                // check if the state has returned to a state where precharge isn't necessary every CHARGE_PAUSE (100ms)
                ThisThread::sleep_for(CHARGE_PAUSE);
            }
            if (!dont_allow_charge) {
                allow_precharge = true;
            }
            continue;
        }
        */
        ThisThread::sleep_for(CHARGE_PAUSE);
    }
    
}

void battery_discharge() {
    // same logic as the method above
    while (true) {
        int contact_status = contact12_input.read();

        //code for testing precharge while pack doesn't work
        //discharge_relay_status = 1;
        if(has_faulted || cell_voltage_fault) {
            discharge_enable.write(0);
            motor_precharge.write(0);
            log_debug("Has faulted");
            //allow_discharge = true;
            continue;
        }    
        else if(discharge_relay_status && contact_status && allow_discharge && !cell_voltage_fault && !has_faulted) {
            has_precharged_discharge_before = true;
            log_debug("Is Precharging Discharge");
            allow_discharge = false;
            start_discharge();
            ThisThread::sleep_for(PRECHARGE_CHARGING);
            discharge_enable.write(1);
            ThisThread::sleep_for(PRECHARGE_OVERLAP);
            motor_precharge.write(0);
            continue;
        }
        /*
        if((!discharge_relay_status || !contact_status) && has_precharged_discharge_before && ! allow_discharge) {
            bool dont_allow_discharge = false;
            chrono::steady_clock::time_point start = chrono::steady_clock::now();
            while(chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start).count() < 30) {
                // TODO: same as above && vs ||
                if(discharge_relay_status && contact_status) {
                    dont_allow_discharge = true;
                    break;
                }
                ThisThread::sleep_for(CHARGE_PAUSE);
            }
            if(!dont_allow_discharge) {
                allow_discharge = true;
            }
            continue;
        }
        */
        ThisThread::sleep_for(CHARGE_PAUSE);
    }
    
}

int main() {
    // Starts precharge and discharge threads and stays active
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");

    precharge_check.start(battery_precharge);
    discharge_check.start(battery_discharge);

    while (true) {
        log_debug("Main thread loop");
        log_debug("aux_input: %d; dc_input: %d; fantech_input: %d; contact12_input: %d; ", aux_input.read(), dcdc_input.read(), fantech_input.read(), contact12_input.read());
        log_debug("mppt_precharge: %d; motor_precharge: %d; discharge_enable: %d; charge_enable: %d\n",  mppt_precharge.read(), motor_precharge.read(), discharge_enable.read(), charge_enable.read());
        if (!contact12V_has_gone_high && contact12_input.read()) {
            contact12V_has_gone_high = true;
        }
        if(!has_faulted) {
            has_faulted = contact12V_has_gone_high && !(contact12_input.read());
        }
        ECUPowerAuxCommands x;
        x.headlights = 1; 
        x.hazards = bms_strobe;
        ThisThread::sleep_for(MAIN_LOOP_PERIOD);
        if (cell_voltage_fault || has_faulted) {
        x.hazards = 1;
        }
        vehicle_can_interface.send(&x);
        if (has_faulted) {
            mppt_precharge.write(0);
            discharge_enable.write(0);
            motor_precharge.write(0);
            charge_enable.write(0);
        }
    }
}

void BPSCANInterface::handle(BPSPackInformation *can_struct) {
    charge_relay_status = can_struct->charge_relay_status;
    discharge_relay_status = can_struct->discharge_relay_status;
    //can_struct->log(LOG_INFO);
}

void BPSCANInterface::handle(BPSError *can_struct) {
    // Checks critical faults are present and sends ECUPowerAux command to turn on bms strobe
    // ECUPowerAuxCommands headlights field will be high if the message is from battery board the hazards field indicates if BMS strobe is high or not

    bms_strobe = can_struct->internal_communications_fault || can_struct-> low_cell_voltage_fault || can_struct->open_wiring_fault || can_struct->current_sensor_fault || can_struct->pack_voltage_sensor_fault || can_struct->thermistor_fault || can_struct->canbus_communications_fault || can_struct->high_voltage_isolation_fault || can_struct->charge_limit_enforcement_fault || can_struct->discharge_limit_enforcement_fault || can_struct->charger_safety_relay_fault || can_struct->internal_thermistor_fault || can_struct->internal_memory_fault;
    if (bms_strobe) {
        has_faulted = true;
    }
    //can_struct->log(LOG_INFO);


    /*
    ECUPowerAuxCommands x;
    x.headlights = 1; 
    x.hazards = bms_strobe;
    if (cell_voltage_fault ||  (contact12V_has_gone_high && !(contact12_input.read()))) {
        x.hazards = 1;
    }
    vehicle_can_interface.send(&x);
    */
}

void BPSCANInterface::handle(BPSCellVoltage *can_struct) {
    //cell_voltage_fault = false;
    //can_struct->log(LOG_DEBUG);
    log_debug("Recieved BPSCellVoltage Frame");
    if (BPS_Cell_Messages < 3) {
        BPS_Cell_Messages++;
    }
    if ((can_struct->low_cell_voltage < low_cell_voltage_threshold || can_struct->high_cell_voltage > high_cell_voltage_threshold) && BPS_Cell_Messages >= 3) {
        if (!has_low_cell_voltage_before) {
            has_low_cell_voltage_before = true;
        }
        else {
            cell_voltage_fault = true;
            log_debug("Low or High cell voltage fault");
        }
        
        //ECUPowerAuxCommands x;
        //x.headlights = 1; 
        //x.hazards = 1;
        //vehicle_can_interface.send(&x);
    }
}


void BPSCANInterface::message_forwarder(CANMessage *message) {
    // message_forwarder is called whenever the BPSCANInterface gets a CAN message
    // this forwards the message to the vehicle can bus
    vehicle_can_interface.send_message(message);
}   