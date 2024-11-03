#include "ECUCANStructs.h"
#include "WheelCANInterface.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>


//Defining Pins
InterruptIn left_signal(LEFT_SIGNAL);
InterruptIn right_signal(RIGHT_SIGNAL);
InterruptIn hazards(HAZARDS);
InterruptIn cruise_control_enable(CRUISE_CONTROL_ENABLE);
InterruptIn cruise_control_inc(CRUISE_CONTROL_INC);
InterruptIn cruise_control_dec(CRUISE_CONTROL_DEC);
InterruptIn regen_enable(REGEN_ENABLE);

WheelCANInterface wheel_board_interface(CAN_RX, CAN_TX, CAN_STBY);
CANStruct wheelcanstruct;
ECUWheelBoardCommands wheel_can_struct;


//Read input pin values
// void readPins(){
//     left_signal.read();
//     right_signal.read();
//     hazards.read();
//     cruise_control_enable.read();
//     cruise_control_inc.read();
//     cruise_control_dec.read();
//     regen_enable.read();
// }

//Poll dashboard buttons & send status over CAN
void send_dashboardCommands(){ 
    wheel_can_struct.left_signal = left_signal.read();
    wheel_can_struct.right_signal = right_signal.read();
    wheel_can_struct.hazards = hazards.read();
    wheel_can_struct.cruise_control_enable = cruise_control_enable.read();
    wheel_can_struct.cruise_control_inc = cruise_control_inc.read();
    wheel_can_struct.cruise_control_dec = cruise_control_dec.read();
    wheel_can_struct.regen_enable = regen_enable.read();
    
    
    //send status via can
    wheel_board_interface.send(&wheel_can_struct);
    
}



int main() {

}