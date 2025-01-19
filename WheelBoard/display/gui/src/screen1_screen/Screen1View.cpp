#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/utils.hpp>
// #include "WheelCANInterface.h"
// #include "Printing.h"
// #include "ThisThread.h"
// #include "ECUCANStructs.h"
// #include "log.h"
// #include "pindef.h"
// #include <mbed.h>
// #include <rtos.h>


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

// const bool PIN_ON = true;
// const bool PIN_OFF = false;

// bool flashHazards, flashLSignal, flashRSignal = false;
// bool regenEnabled = false;
// bool rpmPositive = false;
// bool strobeEnabled = false;
// bool brakeLightsEnabled = false;
// bool regenActive = false;
// bool bms_error = false;
// bool contact_12_error = false;
// bool left_on = false;
// bool left_off = true;
// Thread signalFlashThread;
// Thread motor_thread;





// DigitalOut brake_lights(BRAKE_LIGHTS_OUT);
// DigitalOut leftTurnSignal(LEFT_TURN_OUT);
// DigitalOut rightTurnSignal(RIGHT_TURN_OUT);
// DigitalOut drl(DRL_OUT);
// DigitalOut bms_strobe(BMS_STROBE_OUT);

// DigitalIn brakeLightsSwitch(MECHANICAL_BRAKE_IN);
// DigitalIn leftTurnSwitch(LEFT_TURN_IN);
// DigitalIn rightTurnSwitch(RIGHT_TURN_IN);
// DigitalIn hazardsSwitch(HAZARDS_IN);
// DigitalIn regenSwitch(REGEN_IN);

// //TODO: add pins for cruise control
// DigitalIn cruiseControlSwitch(CRUISE_ENABLED);
// DigitalIn cruiseIncrease(CRUISE_INC);
// DigitalIn cruiseDecrease(CRUISE_DEC);

// AnalogIn throttle(THROTTLE_VALUE_IN, 5.0f);

// DriverCANInterface vehicle_can_interface(CAN_RX, CAN_TX, CAN_STBY);

// ECUMotorCommands to_motor;
// ECUPowerAuxCommands power_aux_out;

// const bool LOG_ECU_POWERAUX_COMMANDS = false;
// const bool LOG_BPS_PACK_INFORMATION = true;
// const bool LOG_BPS_ERROR = false;
// const bool LOG_BPS_CELL_VOLTAGE = false;
// const bool LOG_BPS_CELL_TEMPERATURE = false;
// int RPM = 0;

// bool prevSpeedIncrease = false;
// bool prevSpeedDecrease = false;
// bool cruiseControlEnabled = false;
// bool prevCruiseControlEnabled = false;
// bool prevCruiseControlSwitch = false;
// bool speedIncrease = false;
// bool speedDecrease = false;
// uint16_t currentSpeed = 0;

int random_number = std::rand() % 101;
// bool leftBlinker = false;

Screen1View::Screen1View()
{
    
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::function1()
{
    touchgfx_printf("buttonClicked\n");
    gauge1.updateValue(5, 25);
    gauge1.setValue(5);
    // int rand = std::rand()%2;
    // if(rand == 1)
    // {
    //     leftBlinker = true;
    // }
    // if(leftBlinker)
    // {
        
    // }
}

void Screen1View::function2()
{
    gauge1.updateValue(random_number, 100);
}

void Screen1View::function3()
{
    random_number = std::rand() % 101;
    Unicode::UnicodeChar buffer[8]; // Adjust size if needed
    Unicode::snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), "%d", random_number);
    textArea4.setWildcard(buffer); // Set this buffer to a text area if needed
    textArea4.invalidate();
}

// void Screen1View::function4()
// {

// }