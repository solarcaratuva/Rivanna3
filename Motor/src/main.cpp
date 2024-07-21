#include "MotorCANInterface.h"
#include "MotorControllerCANInterface.h"
#include "MotorInterface.h"
#include "MotorStateTracker.h"
#include "Printing.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>

#define LOG_LEVEL        LOG_ERROR
#define MAIN_LOOP_PERIOD 100ms

EventQueue event_queue(32 * EVENTS_EVENT_SIZE);
Thread event_thread;

MotorCANInterface vehicle_can_interface(MAIN_CAN_RX, MAIN_CAN_TX);

MotorControllerCANInterface motor_controller_can_interface(MTR_CTRL_CAN_RX,
                                                           MTR_CTRL_CAN_TX,
                                                           MTR_CTRL_CAN_STBY);

// Motor Interface
I2C throttle(SDA_ACCEL, SCL_ACCEL);
I2C regen(SDA_REGEN, SCL_REGEN);
DigitalOut gear(FWD_REV_EN);
DigitalOut ignition(MAIN_SWITCH);

MotorInterface motor_interface(throttle, regen, gear, ignition);

// Motor State Tracker
MotorStateTracker motor_state_tracker;

Timeout ECUMotorCommands_timeout;

// If we have not received an ECUMotorCommands struct in 100ms, we assume that
// the CAN bus is down and set the throttle to 0.
void handle_ECUMotorCommands_timeout() { motor_interface.sendThrottle(0x000); }

// RPM + Current
uint16_t rpm, current, currentSpeed;
double _pre_error, _integral;
uint16_t maxAcceleration = 128;
uint16_t previousThrottle = 0;
bool half_throttle = false;

// set values for initalization
double dt, _max, _min, Kp, Kd, Ki;

void init(uint16_t __max, uint16_t __min, double _Kp, double _Ki, double _Kd){
    _max = __max;
    _min = __min;
    Kp = _Kp;
    Ki = _Ki;
    Kd = _Kd;
}

uint16_t calculate(uint16_t setpoint, uint16_t pv){
    // Calculate error
    double error = setpoint - pv;
    
    // Proportional term
    double Pout = Kp * error;

    // Integral term
    _integral += error * dt;
    double Iout = Ki * _integral;

    // Derivative term
    double derivative = (error - _pre_error) / dt;
    double Dout = Kd * derivative;
    uint16_t output = (uint16_t)(Pout + Iout + Dout);

    if( output > _max )
        output = _max;
    else if( output < _min )
        output = _min;
    
    _pre_error = error;
    return output;
}

uint16_t calculateDampenedThrottle(uint16_t setpoint){
    int16_t deltaThrottle = setpoint - previousThrottle;

    if(deltaThrottle > maxAcceleration){
        setpoint = previousThrottle + maxAcceleration;
    }
    previousThrottle = setpoint;

    return setpoint;
}


int main() {
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");
    event_thread.start(callback(&event_queue, &EventQueue::dispatch_forever));

    ECUMotorCommands_timeout.attach(
    event_queue.event(handle_ECUMotorCommands_timeout), 100ms);

    init(256,0, 8, 0.1, 0);
    _pre_error = 0;
    _integral = 0;
    dt =  0.1;
    while (true) {
        motor_controller_can_interface.request_frames(true, true, true);
        ThisThread::sleep_for(MAIN_LOOP_PERIOD);
    }
}

void MotorCANInterface::handle(ECUMotorCommands *can_struct) {
    // Reset current timeout
    ECUMotorCommands_timeout.detach();
    // Set new timeout for 100ms from now
    ECUMotorCommands_timeout.attach(
        event_queue.event(handle_ECUMotorCommands_timeout), 100ms);

    can_struct->log(LOG_INFO);

    motor_interface.sendIgnition(can_struct->motor_on);
    motor_interface.sendDirection(
        can_struct->forward_en); // TODO: verify motor controller will not allow
                                 // gear change when velocity is non-zero
    

    bool cruiseControlEnabled = (can_struct->cruise_control_en); //added to toggle using throttle vs. cc value
    if(cruiseControlEnabled) {
        uint16_t current = calculate(can_struct->cruise_control_speed, currentSpeed);
        uint16_t dampened_current = calculateDampenedThrottle(current);
        //log_error("Sent cc current: %d, cc speed: %d, dampened: %d, integral: %d/1000", current, can_struct->cruise_control_speed, dampened_current, (int)(_integral*1000));
        if(half_throttle) {
          dampened_current >>= 1;
        }
        log_error("target speed: %d, current speed: %d, sent to motor: %d", can_struct->cruise_control_speed, currentSpeed, dampened_current);
        motor_interface.sendThrottle(dampened_current);
    } else {
        _integral = 0;
        uint16_t dampened_current = calculateDampenedThrottle(can_struct->throttle);
        if(half_throttle) {
          dampened_current >>= 1;
        }
        motor_interface.sendThrottle(dampened_current);
    }
    
    motor_interface.sendRegen(can_struct->regen);

    // log_error("R: %d T: %d", can_struct->regen, can_struct->throttle);

}

void MotorControllerCANInterface::handle(
    MotorControllerPowerStatus *can_struct) {
    // can_struct->log(LOG_ERROR);
    rpm = can_struct->motor_rpm;
    current = can_struct->motor_current;
    currentSpeed = (uint16_t)((double)rpm * (double)0.0596); 
    motor_state_tracker.setMotorControllerPowerStatus(*can_struct);
    //log_error("fet temp: %d", can_struct->fet_temp);
}

void MotorControllerCANInterface::handle(
    MotorControllerDriveStatus *can_struct) {
    // can_struct->log(LOG_ERROR);
    // log_error("fwd rev: %d", can_struct->motor_status);
    // log_error("pwr (1), eco (0): %d", can_struct->power_mode);
    half_throttle = can_struct->motor_status == 2 && can_struct->power_mode == 1;
    motor_state_tracker.setMotorControllerDriveStatus(*can_struct);
}

void MotorControllerCANInterface::handle(MotorControllerError *can_struct) {
    // can_struct->log(LOG_ERROR);
    motor_state_tracker.setMotorControllerError(*can_struct);
}


void MotorControllerCANInterface::message_forwarder(CANMessage *message) {
    // message_forwarder is called whenever the MotorControllerCANInterface gets a CAN message
    // this forwards the message to the vehicle can bus
    //log_error("forwarded id: %d", message->id);
    vehicle_can_interface.send_message(message);
}


