// #include "ECUCANStructs.h"
#include "PowerCANInterface.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>
#include <MotorInterface.h>
#include <events/EventQueue.h>
#include "ReadFiles.h"
#include "main.h"

#define LOG_LEVEL                    LOG_ERROR

#define THROTTLE_LOW_VOLTAGE         0.66
#define THROTTLE_LOW_VOLTAGE_BUFFER  0.20
#define THROTTLE_HIGH_VOLTAGE        3.08
#define THROTTLE_HIGH_VOLTAGE_BUFFER 0.10
#define SIGNAL_FLASH_HANDLER_VAL     1s

#define MAX_REGEN 256

#define MOTOR_STATUS_LOOP_PERIOD 10ms

EventQueue queue(32 * EVENTS_EVENT_SIZE);

const bool PIN_ON = true;
const bool PIN_OFF = false;

// Where does Accel pins go?
DigitalOut bms_strobe(NC);
DigitalOut brake_lights(NC);
DigitalOut right_turn_signal(RIGHT_TURN_EN);
DigitalOut left_turn_signal(LEFT_TURN_EN);
DigitalOut drl(DRL_EN);
DigitalOut mppt_precharge(NC);
DigitalOut charge(CHARGE_EN);
DigitalOut motor_precharge(MTR_PRE_EN);
DigitalOut discharge(DIS_CHARGE_EN);

AnalogIn throttle_pedal(THROTTLE_WIPER, 5.0f);
AnalogIn brake_pedal(BRAKE_WIPER, 5.0f);
AnalogIn contactor(CONT_12);
AnalogIn aux_battery(AUX);

I2C throttle(ACCEL_SDA, ACCEL_SCL);
I2C regen(NC, NC);

MotorInterface motor_interface(throttle, regen);

// Need to update powercaninterface
PowerCANInterface vehicle_can_interface(NC, NC, NC);

// Placeholders for DigitalIn pins
bool flashLeftTurnSignal = false;
bool flashRightTurnSignal = false;
bool flashHazards = false;
bool bms_error = false;
bool contact_12_error = false;

// Placeholders
bool has_faulted = false;
bool regen_enabled = false;
bool cruise_control_enabled = false;
bool cruise_control_increase = false;
bool cruise_control_decrease = false;

/**
 * Function that handles the flashing of the turn signals and hazard lights.
 * Reads in the values of the DigitalIn pins from CAN Messages.
 * Writes directly to the DigitalOut pins for the left and right turn signals.
 */
void signal_flash_handler() {
    if (bms_error || contact_12_error) {
        bms_strobe.write(!bms_strobe.read());
    }

    if (flashHazards) {
        left_turn_signal.write(!left_turn_signal.read());
        right_turn_signal.write(left_turn_signal.read());
    } else if (flashLeftTurnSignal) {
        left_turn_signal.write(!left_turn_signal.read());
        right_turn_signal.write(PIN_OFF);
    } else if (flashRightTurnSignal) {
        right_turn_signal.write(!right_turn_signal.read());
        left_turn_signal.write(PIN_OFF);
    } else {
        left_turn_signal.write(PIN_OFF);
        right_turn_signal.write(PIN_OFF);
    }
}


/**
 * Sets the throttle and regen values based on the regen and throttle formula
 */
void regen_drive() {
    uint16_t pedalValue = read_throttle();
    uint16_t throttleValue;
    uint16_t regenValue;

    if (pedalValue <= 50) {
        throttleValue = 0;
        regenValue = 79.159 * pow(50 - pedalValue, 0.3);
    } else if (pedalValue < 100) {
        throttleValue = 0;
        regenValue = 0;
    } else {
        throttleValue = -56.27610464 * pow(156 - (pedalValue - 100), 0.3) + 256;
        regenValue = 0;
    }

    motor_interface.sendThrottle(throttleValue);
    motor_interface.sendRegen(regenValue);
}

/**
 * Function that polls the throttle and brake pedals and sets throttle and regen values
 * Checks if the system has faulted, breaks are enabled, cruise control is enabled, or regen is 
 * enabled and sets the throttle and regen values accordingly
 */
void set_motor_status() {
    if (has_faulted) {
        motor_interface.sendThrottle(0);
        motor_interface.sendRegen(0);
    } else if (read_brake() > 0) {
        motor_interface.sendThrottle(0);
        if (regen_enabled) {
            motor_interface.sendRegen(MAX_REGEN);
        } else {
            motor_interface.sendRegen(0);
        }
    } else if (cruise_control_enabled){
        return;
    } else if(regen_enabled){
        regen_drive();
    } else {
        motor_interface.sendThrottle(read_throttle());
        motor_interface.sendRegen(0);
    }

}


int main() {
    drl.write(PIN_ON);
    queue.call_every(MOTOR_STATUS_LOOP_PERIOD, set_motor_status);
    queue.call_every(SIGNAL_FLASH_HANDLER_VAL, signal_flash_handler);
    queue.dispatch_forever();
}

void PowerCANInterface::handle(DashboardCommands *can_struct){
    flashHazards = can_struct->hazards;
    flashLeftTurnSignal = can_struct->left_turn_signal;
    flashRightTurnSignal = can_struct->right_turn_signal;
    regen_enabled = can_struct->regen_en;
    cruise_control_enabled = can_struct->cruise_en;
    cruise_control_increase = can_struct->cruise_inc;
    cruise_control_decrease = can_struct->cruise_dec;
    queue.call(set_motor_status);
}

void PowerCANInterface::handle(BPSError *can_struct) {
    bms_error = can_struct->internal_communications_fault || can_struct-> low_cell_voltage_fault || can_struct->open_wiring_fault || can_struct->current_sensor_fault || can_struct->pack_voltage_sensor_fault || can_struct->thermistor_fault || can_struct->canbus_communications_fault || can_struct->high_voltage_isolation_fault || can_struct->charge_limit_enforcement_fault || can_struct->discharge_limit_enforcement_fault || can_struct->charger_safety_relay_fault || can_struct->internal_thermistor_fault || can_struct->internal_memory_fault;
}