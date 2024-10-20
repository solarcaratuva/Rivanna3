// #include "ECUCANStructs.h"
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

//Read input pin values
void readPins(){
    left_signal.read();
    right_signal.read();
    hazards.read();
    cruise_control_enable.read();
    cruise_control_inc.read();
    cruise_control_dec.read();
    regen_enable.read();
}

//Poll dashboard buttons & send status over CAN
void send_dashboardCommands(){ 
    
}



int main() {

}