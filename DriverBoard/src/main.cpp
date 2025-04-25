#include "DriverCANInterface.h"
#include "Printing.h"
#include "ThisThread.h"
#include "ECUCANStructs.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>

#define LOG_LEVEL          LOG_ERROR
#define MAIN_LOOP_PERIOD   1s
#define MOTOR_LOOP_PERIOD  10ms
#define ERROR_CHECK_PERIOD 100ms
#define FLASH_PERIOD       500ms
#define IDLE_PERIOD        100ms
#define THROTTLE_LOW_VOLTAGE         0.66
#define THROTTLE_LOW_VOLTAGE_BUFFER  0.20
#define THROTTLE_HIGH_VOLTAGE        3.08
#define THROTTLE_HIGH_VOLTAGE_BUFFER 0.10
#define UPDATE_SPEED 5
#define MIN_SPEED 0
#define MAX_SPEED 50


// PowerAuxCANInterface vehicle_can_interface(MAIN_CAN_RX, MAIN_CAN_TX,
//                                            MAIN_CAN_STBY);
// BPSCANInterface bps_can_interface(BMS_CAN1_RX, BMS_CAN1_TX, BMS_CAN1_STBY);

const bool PIN_ON = true;
const bool PIN_OFF = false;

bool flashHazards, flashLSignal, flashRSignal = false;
bool regenEnabled = false;
bool rpmPositive = false;
bool strobeEnabled = false;
bool brakeLightsEnabled = false;
bool regenActive = false;
bool bms_error = false;
bool contact_12_error = false;
bool left_on = false;
bool left_off = true;
Thread signalFlashThread;
Thread motor_thread;


DigitalOut brake_lights(BRAKE_LIGHTS_OUT);
DigitalOut leftTurnSignal(LEFT_TURN_OUT);
DigitalOut rightTurnSignal(RIGHT_TURN_OUT);
DigitalOut drl(DRL_OUT);
DigitalOut bms_strobe(BMS_STROBE_OUT);
DigitalOut debug_CANBUS(PB_0);

DigitalIn brakeLightsSwitch(MECHANICAL_BRAKE_IN);
DigitalIn leftTurnSwitch(LEFT_TURN_IN);
DigitalIn rightTurnSwitch(RIGHT_TURN_IN);
DigitalIn hazardsSwitch(HAZARDS_IN);
DigitalIn regenSwitch(REGEN_IN);

//TODO: add pins for cruise control
DigitalIn cruiseControlSwitch(CRUISE_ENABLED);
DigitalIn cruiseIncrease(CRUISE_INC);
DigitalIn cruiseDecrease(CRUISE_DEC);

AnalogIn throttle(THROTTLE_VALUE_IN, 5.0f);

DriverCANInterface vehicle_can_interface(CAN_RX, CAN_TX, CAN_STBY);

ECUMotorCommands to_motor;
ECUPowerAuxCommands power_aux_out;
BPSError bps_error;

const bool LOG_ECU_POWERAUX_COMMANDS = false;
const bool LOG_BPS_PACK_INFORMATION = true;
const bool LOG_BPS_ERROR = false;
const bool LOG_BPS_CELL_VOLTAGE = false;
const bool LOG_BPS_CELL_TEMPERATURE = false;
int RPM = 0;

bool prevSpeedIncrease = false;
bool prevSpeedDecrease = false;
bool cruiseControlEnabled = false;
bool prevCruiseControlEnabled = false;
bool prevCruiseControlSwitch = false;
bool speedIncrease = false;
bool speedDecrease = false;
uint16_t currentSpeed = 0;

uint16_t readThrottle() {
    float adjusted_throttle_input =
        ((throttle.read_voltage() - THROTTLE_LOW_VOLTAGE -
          THROTTLE_LOW_VOLTAGE_BUFFER) /
         (THROTTLE_HIGH_VOLTAGE - THROTTLE_HIGH_VOLTAGE_BUFFER -
          THROTTLE_LOW_VOLTAGE - THROTTLE_LOW_VOLTAGE_BUFFER));
    if (adjusted_throttle_input <= 0.0f) {
        return 0;
    } else if (adjusted_throttle_input >= 1.0f) {
        return 256;
    } else {
        return (uint16_t)(adjusted_throttle_input * 256.0);
    }
}

void read_inputs() {
    flashHazards = hazardsSwitch.read();
    flashLSignal = leftTurnSwitch.read();
    flashRSignal = rightTurnSwitch.read();
    regenEnabled = regenSwitch.read();
    brakeLightsEnabled = brakeLightsSwitch.read();

    // if(cruiseControlSwitch) {
    //     log_debug("cruiseControlSwitch pressed");
    // }
    // cruiseControlSwitch ? log_debug("CC switch pressed") : log_debug("CC switch not pressed");
    // cruiseIncrease ? log_debug("CC increase switch pressed") : log_debug("CC increase switch not pressed");
    // cruiseDecrease ? log_debug("CC decrease switch pressed") : log_debug("CC decrease switch not pressed");
    
    //log_debug(cruiseControlSwitch);
    // log_debug(cruiseDecrease);
    // log_debug(cruiseIncrease);
    // log_debug(regenEnabled);
    // log_debug(flashLSignal);
    // log_debug(flashRSignal);
    // log_debug(flashHazards);
  
    speedIncrease = cruiseIncrease.read();
    speedDecrease = cruiseDecrease.read();
}

void signalFlashHandler() {
    while (true) {
        // Note: Casting from a `DigitalOut` to a `bool` gives the most recently written value
        if(bms_error || contact_12_error) {
            bms_strobe = !bms_strobe;
        }

        brake_lights = brakeLightsEnabled || regenActive;

        if (flashHazards) {
            bool leftTurnSignalState = leftTurnSignal;
            leftTurnSignal = !leftTurnSignalState;
            rightTurnSignal = leftTurnSignalState;
            brake_lights = PIN_OFF;
        } else if (flashLSignal) {
            leftTurnSignal = !leftTurnSignal;
            rightTurnSignal = PIN_OFF;
            brake_lights = PIN_OFF;
        } else if (flashRSignal) {
            leftTurnSignal = left_off;
            rightTurnSignal = !rightTurnSignal;
            brake_lights = PIN_OFF;
        } else {
            leftTurnSignal = left_off;
            rightTurnSignal = PIN_OFF;
            brake_lights = PIN_OFF;
        }
        ThisThread::sleep_for(FLASH_PERIOD);
    }
}

//Moved motor control from main loop (1s) to it's own loop (10ms)
void motor_message_handler(){
    while(true){
        uint16_t pedalValue = readThrottle();
        uint16_t regenValue;    
        uint16_t throttleValue;

        if (regenEnabled) {
            // One pedal drive (tesla style)
            if (pedalValue <= 50) {
                throttleValue = 0;
                regenValue = 79.159 * pow(50-pedalValue, 0.3);
            } else if (pedalValue < 100) {
                throttleValue = 0;
                regenValue = 0;
            } else {
                throttleValue = -56.27610464*pow(156-(pedalValue-100),0.3) + 256;
                regenValue = 0;
            }
        } else {
            if(pedalValue <= 50) {
                throttleValue = 0;
            } else {
                throttleValue = pedalValue;
            }
            regenValue = 0;
        }

        regenActive = regenValue > 0;

        to_motor.throttle = throttleValue;

        bool cruiseControlRisingEdge = cruiseControlSwitch && !prevCruiseControlSwitch;
        bool cruiseControlFallingEdge = !cruiseControlSwitch && prevCruiseControlSwitch;

        // if(cruiseControlRisingEdge) {
        //     log_error("cc switch rising edge");
        // }

        // if(cruiseControlFallingEdge) {
        //     log_error("cc switch falling edge");
        // }

        // if(brakeLightsSwitch) {
        //     log_error("brake switch on");
        // }

        if(brakeLightsSwitch || regenEnabled){
            cruiseControlEnabled = false;
            // log_error("brake or throttle nonzero");
        } else if(cruiseControlRisingEdge){
            cruiseControlEnabled = true;
        } else if(cruiseControlFallingEdge){
            cruiseControlEnabled = false;
        }
        prevCruiseControlSwitch = cruiseControlSwitch;
            
        bool increaseRisingEdge = speedIncrease and !prevSpeedIncrease;
        bool decreaseRisingEdge = speedDecrease and !prevSpeedDecrease;

        // if(increaseRisingEdge) {
        //     log_error("increase rising");
        // }
        // if(decreaseRisingEdge) {
        //     log_error("decrease rising");
        // }
      
        prevSpeedIncrease = speedIncrease;
        prevSpeedDecrease = speedDecrease;
      
        to_motor.cruise_control_en = cruiseControlEnabled;
    
        if(cruiseControlEnabled and !prevCruiseControlEnabled){
            //double curr = (double)((RPM * 3.1415926535 * 16 * 60)/(63360));
            //currentSpeed = curr/5*5;
            currentSpeed  = ((double) RPM) * ((double) 0.0596);
            // log_error("cc rising, set speed to %d, RPM=%d", currentSpeed, RPM);
            to_motor.cruise_control_speed = currentSpeed;
        } else{
            if(increaseRisingEdge and decreaseRisingEdge){
            } else if(increaseRisingEdge){
                to_motor.cruise_control_speed = min(MAX_SPEED,  currentSpeed + UPDATE_SPEED);
                currentSpeed = to_motor.cruise_control_speed;
            } else if(decreaseRisingEdge){
                to_motor.cruise_control_speed = max(MIN_SPEED, currentSpeed - UPDATE_SPEED);
                currentSpeed = to_motor.cruise_control_speed;
            }
        }
        prevCruiseControlEnabled = cruiseControlEnabled;
        // log_error("cc speed: %d, cc en %d, pedal throttle: %d", currentSpeed, cruiseControlEnabled, throttleValue);
        to_motor.regen = regenValue;

        to_motor.forward_en = true;
        to_motor.reverse_en = false; 

        to_motor.motor_on = true;
        vehicle_can_interface.send(&to_motor);
        to_motor.log(LOG_DEBUG);

        ThisThread::sleep_for(MOTOR_LOOP_PERIOD);
    }

}

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");
    
    while (true){
        log_debug("Main thread loop");
        // vehicle_can_interface.send(&bps_error);
        ThisThread::sleep_for(MOTOR_LOOP_PERIOD);
    }

    // motor_thread.start(motor_message_handler);
    // signalFlashThread.start(signalFlashHandler);

    // drl = PIN_ON;

    // while (true) {
    //     log_debug("Main thread loop");
    //     CANMessage message;
    //     if (CANRead(message) == 1){

    //     }
        // read_inputs();

    //     to_motor.throttle = 1;
    //     to_motor.cruise_control_speed = 16;

    //     vehicle_can_interface.send(&to_motor);

    //     ThisThread::sleep_for(MAIN_LOOP_PERIOD);

    //     //  hazards, brake_lights, headlights, left_turn_signal,
    //     //              right_turn_signal
    //     power_aux_out.hazards = true;
    //     power_aux_out.brake_lights = true;
    //     power_aux_out.headlights = 0;
    //     power_aux_out.left_turn_signal = true;
    //     power_aux_out.right_turn_signal = true;

    //     vehicle_can_interface.send(&power_aux_out);
    // }
}

void DriverCANInterface::handle(MotorControllerPowerStatus *can_struct) {
    // rpmPositive = can_struct->motor_rpm > 0;
    vehicle_can_interface.send(can_struct);
    //log_error("sent rpm: %d", can_struct->motor_rpm);
    RPM = can_struct->motor_rpm; 
}

void DriverCANInterface::handle(BPSError *can_struct) {
    bms_error = can_struct->internal_communications_fault || can_struct-> low_cell_voltage_fault || can_struct->open_wiring_fault || can_struct->current_sensor_fault || can_struct->pack_voltage_sensor_fault || can_struct->thermistor_fault || can_struct->canbus_communications_fault || can_struct->high_voltage_isolation_fault || can_struct->charge_limit_enforcement_fault || can_struct->discharge_limit_enforcement_fault || can_struct->charger_safety_relay_fault || can_struct->internal_thermistor_fault || can_struct->internal_memory_fault;
    debug_CANBUS = PIN_ON;
    ThisThread::sleep_for(MAIN_LOOP_PERIOD);
    debug_CANBUS = PIN_OFF;
}

void DriverCANInterface::handle(ECUPowerAuxCommands *can_struct) {
    if(can_struct->headlights && can_struct->hazards) {
        contact_12_error = true;
    }
}