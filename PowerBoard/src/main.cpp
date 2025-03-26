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

#define LOG_LEVEL                       LOG_DEBUG
#define SIGNAL_FLASH_PERIOD             1s
#define BRAKE_LIGHTS_UPDATE_PERIOD      10ms
#define MOTOR_CONTROL_PERIOD            10ms
#define MOTOR_REQUEST_FRAMES_PERIOD     10ms

#define MAX_REGEN                       256

EventQueue queue(32 * EVENTS_EVENT_SIZE);

constexpr bool PIN_ON = true;
constexpr bool PIN_OFF = false;

DigitalOut bms_strobe(STROBE_EN);
DigitalOut brake_lights(BRAKE_LIGHT_EN);
DigitalOut right_turn_signal(RIGHT_TURN_EN);
DigitalOut left_turn_signal(LEFT_TURN_EN);
DigitalOut drl(DRL_EN);
DigitalOut mppt_precharge(MPPT_PRE_EN);
DigitalOut charge(CHARGE_EN);
DigitalOut motor_precharge(MTR_PRE_EN);
DigitalOut discharge(DIS_CHARGE_EN);

AnalogIn throttle_pedal(THROTTLE_WIPER, 5.0f);
AnalogIn brake_pedal(BRAKE_WIPER, 5.0f);
AnalogIn contactor(CONT_12);
AnalogIn aux_battery(AUX);

I2C motor_control_serial_bus(MTR_SDA, MTR_SCL);
MotorInterface motor_interface(motor_control_serial_bus);

PowerCANInterface vehicle_can_interface(MAIN_CAN_RX, MAIN_CAN_TX, MAIN_CAN_STBY);
MotorControllerCANInterface motor_controller_can_interface(MTR_CAN_RX, MTR_CAN_TX, NC); // TODO add standby pin

// these are global control variables, mostly set by received CAN messages
bool flashLeftTurnSignal = false;
bool flashRightTurnSignal = false;
bool flashHazards = false;
bool bms_error = false;
bool contact_12_error = false; //TODO currently does nothing
bool has_faulted = false; // true if there is any fault that locks the car until reset
bool regen_enabled = false;
bool cruise_control_enabled = false;
bool cruise_control_brake_latch = false;

// CruiseControl cruise_control;

// HeartBeatSystem heartbeatSystem(fault_occurred, &queue, HB_POWER_BOARD);

/**
 * Function that handles the flashing of the turn signals and hazard lights.
 * Uses values of global control variables for logic.
 * Writes directly to the DigitalOut pins for the left and right turn signals.
 */
void signal_flash_handler() {
    if (bms_error || contact_12_error) {
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
    uint16_t brake = read_brake();
    uint16_t regen = 0;

    motor_CAN_struct.throttle_pedal = throttle;
    motor_CAN_struct.brake_pedal = brake;

    if (has_faulted) { // fault case
        throttle = 0;
        brake = 0;
    } else if (brake > 0) { // brake case
        throttle = 0;
        if (regen_enabled) {
            regen = MAX_REGEN;
        } else {
            regen = 0;
        }
        cruise_control_brake_latch = true;
        motor_CAN_struct.braking = true;
    } else if (cruise_control_enabled && !cruise_control_brake_latch) { // cruise control case, logic handled elsewhere
        // TODO: get throttle
        // motor_CAN_struct.cruise_speed = motor_CAN_struct.cruise_drive ? cruise_control.get_cruise_target() : 0;
        motor_CAN_struct.cruise_drive = true;
    } else if(regen_enabled) { // regen drive case
        regen_drive(&motor_CAN_struct, &throttle, &regen);
        motor_CAN_struct.throttle = throttle;
        motor_CAN_struct.regen_braking = regen;
        motor_CAN_struct.regen_drive = true;
    } else { // normal drive case
        // do nothing; throttle, brake, and regen are already set
        motor_CAN_struct.manual_drive = true;
    }

    motor_interface.sendThrottle(throttle);
    motor_interface.sendRegen(regen);

    vehicle_can_interface.send(&motor_CAN_struct);
}


/**
 * Function that sets the brake lights based on the brake pedal value.
 * If the brake pedal is pressed, or regen is enabled and actively regenerative braking, the brake lights are enabled, disabled otherwise
 */
void set_brake_lights(){
    bool brakeLightsEnable = read_brake() > 0 || (regen_enabled && read_throttle() <= 50);
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
    // HeartBeat power_board_hb = heartbeatSystem.send_heartbeat();
    // vehicle_can_interface.send(&power_board_hb);
}

// main method
int main() {
    log_set_level(LOG_LEVEL);
    log_info("PowerBoard starting up");

    while(true) {
        // AnalogIn throttle_pedal(THROTTLE_WIPER, 5.0f);
        // AnalogIn brake_pedal(BRAKE_WIPER, 5.0f);
        // AnalogIn contactor(CONT_12);
        // AnalogIn aux_battery(AUX);
        log_error("throttle pedal: %0.3f, brake pedal: %0.3f, contactor: %0.3f, aux battery: %0.3f",
            throttle_pedal.read(), brake_pedal.read(), contactor.read(), aux_battery.read());
    }
}

// CAN Message handlers

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

    // if(can_struct->cruise_inc) {
    //     cruise_control.increase_cruise_target();
    // }
    // if(can_struct->cruise_dec) {
    //     cruise_control.decrease_cruise_target();
    // }
    
    queue.call(set_motor_status);
}

// Handle heartbeat message
void PowerCANInterface::handle(HeartBeat *can_struct){
    // heartbeatSystem.refreshTimer(can_struct);
}

// Message_forwarder is called whenever the MotorControllerCANInterface gets a CAN message.
// This forwards the message to the vehicle can bus.
void MotorControllerCANInterface::message_forwarder(CANMessage *message) {
    vehicle_can_interface.send_message(message);
}

void MotorControllerCANInterface::handle(MotorControllerPowerStatus *can_struct) {
    uint16_t motor_rpm = can_struct->motor_rpm;
    // cruise_control.send_cruise_control_to_motor(motor_rpm);
}

void MotorControllerCANInterface::handle(MotorControllerDriveStatus *can_struct) {
    // we don't currently do anything with this CAN message
}

// CAN Message error handlers 

void PowerCANInterface::handle(BPSError *can_struct) {
    bms_error = can_struct->internal_communications_fault || can_struct-> low_cell_voltage_fault || can_struct->open_wiring_fault || can_struct->current_sensor_fault || can_struct->pack_voltage_sensor_fault || can_struct->thermistor_fault || can_struct->canbus_communications_fault || can_struct->high_voltage_isolation_fault || can_struct->charge_limit_enforcement_fault || can_struct->discharge_limit_enforcement_fault || can_struct->charger_safety_relay_fault || can_struct->internal_thermistor_fault || can_struct->internal_memory_fault;
    if (bms_error) {
        can_struct->log(LOG_ERROR);
        fault_occurred();
    }
}

void MotorControllerCANInterface::handle(MotorControllerError *can_struct) {
    can_struct->log(LOG_ERROR);
    fault_occurred();
}
