#include "PowerCANInterface.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>
#include <MotorInterface.h>
#include <events/EventQueue.h>
#include "ReadPedals.h"
#include "main.h"
#include "MotorCommandsCANStruct.h"
#include "MotorControllerCANInterface.h"
#include "HeartBeatSystem.h"
#include "CruiseControl.h"
#include "Precharge.h"
#include "AUXBattery.h"
#include "CANStructs.h"


#define LOG_LEVEL                       LOG_DEBUG
#define SIGNAL_FLASH_PERIOD             500ms
#define BRAKE_LIGHTS_UPDATE_PERIOD      10ms
#define MOTOR_CONTROL_PERIOD            10ms
#define MOTOR_REQUEST_FRAMES_PERIOD     10ms
#define AUX_BATTERY_PERIOD              1s
#define MAX_REGEN                       256
#define FLASH_PERIOD       500ms


const bool PIN_ON = true;
const bool PIN_OFF = false;

EventQueue queue(32 * EVENTS_EVENT_SIZE);

DigitalOut bms_strobe(STROBE_EN);
DigitalOut brake_lights(BRAKE_LIGHT_EN);
DigitalOut right_turn_signal(RIGHT_TURN_EN);
DigitalOut left_turn_signal(LEFT_TURN_EN);
DigitalOut drl(DRL_EN);
DigitalOut mppt_precharge_en(MPPT_PRE_EN);
DigitalOut charge_en(CHARGE_EN);
DigitalOut motor_precharge_en(MTR_PRE_EN);
DigitalOut discharge_en(DIS_CHARGE_EN);
DigitalOut LED2_PIN(PB_7);
DigitalOut gpioOutput(PC_10);

DigitalIn gpioInput(PC_11);
DigitalIn brake_pedal(BRAKE_WIPER);


AnalogIn throttle_pedal(THROTTLE_WIPER, 3.3f);
AnalogIn aux_battery(AUX, 3.3f);
AnalogIn hal_effect_voltage_motor(MTR_HAL_SENSE, 3.3f);
AnalogIn hal_effect_voltage_mppt(MPPT_HAL_SENSE, 3.3f);
AnalogIn cont_12(CONT_12, 3.3f);

AnalogIn hil_testing_pin_analog(PC_5, 3.3f);


#define I2C_TEST_MODE 0 //0 = Throttle, 1 = Regen
// I2C motor_control_serial_bus(I2C_SDA1, I2C_SCL1); // comment out for implementation with powerboard
I2C motor_control_serial_bus(MTR_SDA, MTR_SCL); // comment in for implementation with powerboard
MotorInterface motor_interface(motor_control_serial_bus);

PowerCANInterface vehicle_can_interface(CAN_RX, CAN_TX, CAN_STBY);
MotorControllerCANInterface motor_controller_can_interface(MTR_CAN_RX, MTR_CAN_TX, MAIN_CAN_STBY);

BPSError bps_error;

// these are global control variables, mostly set by received CAN messages
bool flashLeftTurnSignal = false;
bool flashRightTurnSignal = false;
bool flashHazards = false;
bool bms_error = false;
bool cont_12_error = false;
bool has_faulted = false; // true if there is any fault that locks the car until reset
bool regen_enabled = false;
bool cruise_control_enabled = false;
bool cruise_control_brake_latch = false;
bool discharge_relay_status = false;
bool charge_relay_status = false;
double pack_voltage = 0.0;

CruiseControl cruise_control;
Thread motor_precharge_thread;
Thread mppt_precharge_thread;

HeartBeatSystem heartbeatSystem(fault_occurred, &queue, HB_POWER_BOARD);

/**
 * Function that handles the flashing of the turn signals and hazard lights.
 * Uses values of global control variables for logic.
 * Writes directly to the DigitalOut pins for the left and right turn signals.
 */
void signal_flash_handler() {
    if (bms_error || cont_12_error) {
        bms_strobe.write(!bms_strobe.read());
    }

    if (flashHazards || has_faulted) {
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
 * Sets the throttle and regen values of the motor based on the regen and throttle formula
 */
void regen_drive(MotorCommands *motor_CAN_struct, uint16_t* throttleValue, uint16_t* regenValue) {
    uint16_t pedalValue = *throttleValue;

    if (pedalValue <= 50) {
        *throttleValue = 0;
        *regenValue = 79.159 * pow(50 - pedalValue, 0.3);
        motor_CAN_struct->braking = true;
    } else if (pedalValue < 100) {
        *throttleValue = 0;
        *regenValue = 0;
    } else {
        *throttleValue = -56.27610464 * pow(156 - (pedalValue - 100), 0.3) + 256;
        *regenValue = 0;
    }
}


/**
 * Function that polls the throttle and brake pedals and sets throttle and regen values
 * Checks if the system has faulted, breaks are enabled, cruise control is enabled, or regen is 
 * enabled and sets the throttle and regen values accordingly
 */
void set_motor_status() {
    MotorCommands motor_CAN_struct = MotorCommands(); // constructor sets all values to 0
    uint16_t throttle = read_throttle();
    bool brake = brake_pedal.read();
    uint16_t regen = 0;

    motor_CAN_struct.throttle_pedal = throttle;
    motor_CAN_struct.brake_pedal = brake;

    if (has_faulted) { // fault case
        throttle = 0;
        regen = 0;
    // } else if (brake) { // brake case
    //     throttle = 0;
    //     if (regen_enabled) {
    //         regen = 256; // max regen
    //     } else {
    //         regen = 0;
    //     }
    //     cruise_control_brake_latch = true;
    //     motor_CAN_struct.braking = true;
    } else if (cruise_control_enabled && !cruise_control_brake_latch) { // cruise control case, logic handled elsewhere
        // TODO: get throttle
        motor_CAN_struct.cruise_drive = true;
        motor_CAN_struct.cruise_speed = motor_CAN_struct.cruise_drive ? cruise_control.get_cruise_target() : 0;
    } else if(regen_enabled) { // regen drive case
        regen_drive(&motor_CAN_struct, &throttle, &regen);
        motor_CAN_struct.regen_braking = regen;
        if (regen > 0)
            motor_CAN_struct.braking = true;
        motor_CAN_struct.regen_drive = true;
    } else { // normal drive case
        // do nothing; throttle, brake, and regen are already set
        motor_CAN_struct.manual_drive = true;
    }

    motor_interface.sendThrottle(throttle);
    motor_interface.sendRegen(regen);

    motor_CAN_struct.throttle = throttle;
    vehicle_can_interface.send(&motor_CAN_struct);
}


/**
 * Function that sets the brake lights based on the brake pedal value.
 * If the brake pedal is pressed, or regen is enabled and actively regenerative braking, the brake lights are enabled, disabled otherwise
 */
void set_brake_lights(){
    bool brakeLightsEnable = brake_pedal.read() || (regen_enabled && read_throttle() <= 50);
    brake_lights.write(brakeLightsEnable);
}

// request new data from the motor
void request_motor_frames() {
    motor_controller_can_interface.request_frames(true, true, true);
}

// call when a fault occurs
void fault_occurred() {
    log_error("A fault occurred! Now putting the car in a safe state.");
    has_faulted = true;
    set_motor_status();
}

// Function that when called creates and sends a Heartbeat can message from PowerBoard
void send_powerboard_heartbeat() {
    Heartbeat power_board_hb = heartbeatSystem.send_heartbeat();
    vehicle_can_interface.send(&power_board_hb);
}

// main method
int main() {
    CANMessage message;

    /* Start : Simple I2C Test*/
    log_debug("I2C Testing");
    // Test throttle values from 0 to 256
    if (I2C_TEST_MODE == 0) {
        for(int i = 0; i <= 256; i += 32) {
            motor_interface.sendThrottle(i);
            ThisThread::sleep_for(FLASH_PERIOD);
        } 
    } else if (I2C_TEST_MODE ==1){
    // Test regen values from 0 to 256
    for(int i =0; i <= 256; i += 32) {
        motor_interface.sendRegen(i);
        ThisThread::sleep_for(FLASH_PERIOD);
    }
    } else {
        log_error("Invalid I2C test mode");
    }
    log_debug("Finished Testing I2C");

    /* End : Simple I2C Test*/


    char buffer[64];
    //Testing Analog
    while (true){
        log_debug("START OF INFINITE LOOP");
        ThisThread::sleep_for(FLASH_PERIOD);

        sprintf(buffer, "%f", hil_testing_pin_analog.read());
        log_debug(buffer);
    }

    //Testing Digital and CAN
    while (true){
        log_debug("SOMETHING");
        //Testing CAN
        LED2_PIN = PIN_ON;
        //Reading from Raspberry Pi to Nucleo
        log_debug("HERE");
        if (vehicle_can_interface.CANRead(message)){
            ThisThread::sleep_for(FLASH_PERIOD);
            LED2_PIN = PIN_OFF;
            ThisThread::sleep_for(FLASH_PERIOD);
        }
        log_debug("BEFORE");
        vehicle_can_interface.send(&bps_error); //Uncomment to send to Raspberry Pi
        log_debug("AFTER");
    }
}

// CAN Message handlers

// Handle heartbeat message
void PowerCANInterface::handle(Heartbeat *can_struct){
    heartbeatSystem.refreshTimer(can_struct);
}

// DashboardCommands CAN message handler
void PowerCANInterface::handle(DashboardCommands *can_struct){
    flashHazards = can_struct->hazards;
    flashLeftTurnSignal = can_struct->left_turn_signal;
    flashRightTurnSignal = can_struct->right_turn_signal;
    regen_enabled = can_struct->regen_en;

    if(can_struct->cruise_en && !cruise_control_enabled) {
        cruise_control_brake_latch = false;
    }

    cruise_control_enabled = can_struct->cruise_en;

    if(can_struct->cruise_inc) {
        cruise_control.increase_cruise_target();
    }
    if(can_struct->cruise_dec) {
        cruise_control.decrease_cruise_target();
    }
    
    queue.call(set_motor_status);
}

void PowerCANInterface::handle(BPSPackInformation *can_struct) {
    discharge_relay_status = can_struct->discharge_relay_status;
    charge_relay_status = can_struct->charge_relay_status;
    pack_voltage = can_struct->pack_voltage / 10.0;
}

// Message_forwarder is called whenever the MotorControllerCANInterface gets a CAN message.
// This forwards the message to the vehicle can bus.
void MotorControllerCANInterface::message_forwarder(CANMessage *message) {
    vehicle_can_interface.send_message(message);
}

void MotorControllerCANInterface::handle(MotorControllerPowerStatus *can_struct) {
    uint16_t motor_rpm = can_struct->motor_rpm;
    cruise_control.send_cruise_control_to_motor(motor_rpm);
}

void MotorControllerCANInterface::handle(MotorControllerDriveStatus *can_struct) {
    // we don't currently do anything with this CAN message
}

void PowerCANInterface::handle(ChargingMode *can_struct) {
    if (can_struct->charging_mode_enable) {
        drl.write(PIN_OFF);
    }
    else {
        drl.write(PIN_ON);
    }
}

// CAN Message error handlers 

// BPSError CAN message handler
void PowerCANInterface::handle(BPSError *can_struct) {
    bms_error = can_struct->dtc_p0_a1_f_internal_cell_communication_fault || can_struct->current_sensor_fault || can_struct->weak_pack_fault || can_struct->thermistor_fault || can_struct->can_communication_fault || can_struct->redundant_power_supply_fault || can_struct->high_voltage_isolation_fault || can_struct->charge_enable_relay_fault || can_struct->discharge_enable_relay_fault || can_struct->internal_hardware_fault || can_struct->dtc_p0_a0_a_internal_heatsink_thermistor_fault || can_struct->internal_logic_fault || can_struct->dtc_p0_a0_c_highest_cell_voltage_too_high_fault || can_struct->dtc_p0_a0_e_lowest_cell_voltage_too_low_fault || can_struct->pack_too_hot_fault || can_struct->low_cell_voltage_fault;
    if (bms_error) {
        can_struct->log(LOG_ERROR);
        fault_occurred();
    }
}

void MotorControllerCANInterface::handle(MotorControllerError *can_struct) {
    if (can_struct->analog_sensor_err || can_struct->motor_current_sensor_u_err || can_struct->motor_current_sensor_w_err || can_struct->fet_thermistor_err || can_struct->battery_voltage_sensor_err || can_struct->battery_current_sensor_err || can_struct->battery_current_sensor_adj_err || can_struct->motor_current_sensor_adj_err || can_struct->accelerator_position_err || can_struct->controller_voltage_sensor_err || can_struct->power_system_err || can_struct->overcurrent_err || can_struct->overvoltage_err || can_struct->overcurrent_limit || can_struct->motor_system_err || can_struct->motor_lock || can_struct->hall_sensor_short || can_struct->hall_sensor_open || can_struct->overheat_level > 0) {
        can_struct->log(LOG_ERROR);
        fault_occurred();
    }
}


