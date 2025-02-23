//#include "ECUCANStructs.h"
#include "WheelCANInterface.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>
#include "DashboardCommandsCANStruct.h"
#include "events/EventQueue.h"

#define LOG_LEVEL                       LOG_DEBUG

EventQueue queue(32 * EVENTS_EVENT_SIZE);

//Defining Pins
InterruptIn left_signal(LEFT_SIGNAL); // toggle
InterruptIn right_signal(RIGHT_SIGNAL); // toggle
InterruptIn hazards(HAZARDS); // toggle
InterruptIn cruise_control_enable(CRUISE_CONTROL_ENABLE); // toggle
InterruptIn cruise_control_inc(CRUISE_CONTROL_INC); // press
InterruptIn cruise_control_dec(CRUISE_CONTROL_DEC); // press
InterruptIn regen_enable(REGEN_ENABLE); // toggle

WheelCANInterface wheel_board_interface(CAN_RX, CAN_TX, CAN_STBY);

Timeout PowerBoard_timeout;


void handle_powerboard_timeout() { printf("Power Board Timeout Detected"); }


/*
void send_wheelboard_heartbeat() {
    HeartBeat wheelboard_heartbeat_struct;
    wheelboard_heartbeat_struct.FromTelemetryBoard = 0;
    wheelboard_heartbeat_struct.FromWheelBoard = 1;
    wheelboard_heartbeat_struct.FromPowerBoard = 0;
    vehicle_can_interface.send(&wheelboard_heartbeat_struct);
}
*/


// Handle heartbeat message from powerboard
void WheelCANInterface::handle(HeartBeat *can_struct){
    if (can_struct->FromPowerBoard == 1 && can_struct->FromWheelBoard == 0 && can_struct->FromTelemetryBoard == 0) {
        // Reset current timeout
        PowerBoard_timeout.detach();
        // Set new timeout for 100ms from now
        PowerBoard_timeout.attach(
        queue.event(handle_powerboard_timeout), 100ms); // was event_queue in original motor main.cpp
    }
}

//Poll dashboard buttons & send status over CAN
void send_DashboardCommands_CAN_message(){ 
    DashboardCommands wheel_can_struct;

    wheel_can_struct.left_turn_signal = left_signal.read();
    wheel_can_struct.right_turn_signal = right_signal.read();
    wheel_can_struct.hazards = hazards.read();
    wheel_can_struct.cruise_en = cruise_control_enable.read();
    wheel_can_struct.cruise_inc = cruise_control_inc.read();
    wheel_can_struct.cruise_dec = cruise_control_dec.read();
    wheel_can_struct.regen_en = regen_enable.read();
    
    
    //send status via CAN
    wheel_board_interface.send(&wheel_can_struct);
    
}

void edge_handler(void){
    queue.call(send_DashboardCommands_CAN_message);
}


int main() {
    log_set_level(LOG_LEVEL);

    // set initial heartbeat timer (Call handle_powerborad_timeout in 100ms)
    PowerBoard_timeout.attach(
    queue.event(handle_powerboard_timeout), 100ms); // was event_queue in original motor main.cpp
    
    // toggle buttons have a rise and fall interrupt, press buttons only have a rise interrupt
    left_signal.rise(edge_handler);
    right_signal.rise(edge_handler);
    hazards.rise(edge_handler);
    cruise_control_enable.rise(edge_handler);
    cruise_control_inc.rise(edge_handler);
    cruise_control_dec.rise(edge_handler);
    regen_enable.rise(edge_handler);

    left_signal.fall(edge_handler);
    right_signal.fall(edge_handler);
    hazards.fall(edge_handler);
    cruise_control_enable.fall(edge_handler);
    regen_enable.fall(edge_handler);

    //queue.
    queue.dispatch_forever();
}