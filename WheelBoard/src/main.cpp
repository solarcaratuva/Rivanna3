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

//Poll dashboard buttons & send status over CAN
void send_DashboardCommands_CAN_message(bool cruise_inc, bool cruise_dec){ 
    DashboardCommands wheel_can_struct;

    wheel_can_struct.left_turn_signal = left_signal.read();
    wheel_can_struct.right_turn_signal = right_signal.read();
    wheel_can_struct.hazards = hazards.read();
    wheel_can_struct.cruise_en = cruise_control_enable.read();
    wheel_can_struct.cruise_inc = cruise_inc;
    wheel_can_struct.cruise_dec = cruise_dec;
    wheel_can_struct.regen_en = regen_enable.read();
    
    
    //send status via CAN
    wheel_board_interface.send(&wheel_can_struct);
    
}

void edge_handler(void){
    queue.call(send_DashboardCommands_CAN_message, false, false);
}

void cruise_control_increase_rise() {
    queue.call(send_DashboardCommands_CAN_message, true, false);
}

void cruise_control_decrease_rise() {
    queue.call(send_DashboardCommands_CAN_message, false, true);
}


int main() {
    log_set_level(LOG_LEVEL);

    // toggle buttons have a rise and fall interrupt, press buttons only have a rise interrupt
    left_signal.rise(edge_handler);
    right_signal.rise(edge_handler);
    hazards.rise(edge_handler);
    cruise_control_enable.rise(edge_handler);
    regen_enable.rise(edge_handler);

    // For cruise buttons, as they are toggles, only send true for one message on rising edge, send false otherwise
    cruise_control_inc.rise(cruise_control_increase_rise);
    cruise_control_dec.rise(cruise_control_decrease_rise);

    left_signal.fall(edge_handler);
    right_signal.fall(edge_handler);
    hazards.fall(edge_handler);
    cruise_control_enable.fall(edge_handler);
    regen_enable.fall(edge_handler);

    queue.dispatch_forever();
}