// #include "ECUCANStructs.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>

#define LOG_LEVEL                    LOG_ERROR

const bool PIN_ON = true;
const bool PIN_OFF = false;

// Where does Accel pins go?
DigitalOut bms_strobe(STROBE_EN);
DigitalOut brake_lights(BRAKE_LIGHT_EN);
DigitalOut right_turn_signal(RIGHT_TURN_EN);
DigitalOut left_turn_signal(LEFT_TURN_EN);
DigitalOut drl(DRL_EN);
DigitalOut mppt_precharge(MPPT_PRE_EN);
DigitalOut charge(CHARGE_EN);
DigitalOut motor_precharge(MTR_PRE_EN);
DigitalOut discharge(DIS_CHARGE_EN);

DigitalIn regen_sda(REGEN_SDA);
DigitalIn regen_scl(REGEN_SCL);

AnalogIn throttle_pedal(THROTTLE_WIPER);
AnalogIn brake_pedal(BRAKE_WIPER);
AnalogIn contactor(CONT_12);
AnalogIn aux_battery(AUX);

// Need to update powercaninterface
PowerCANInterface vehicle_can_interface(UART5_RX, UART5_TX, DEBUG_SWITCH);

// Placeholders for DigitalIn pins
bool flashLeftTurnSignal = false;
bool flashRightTurnSignal = false;
bool flashHazards = false;
bool bms_error = false;
bool contact_12_error = false;

/**
 * Function that handles the flashing of the turn signals and hazard lights.
 * Reads in the values of the DigitalIn pins from CAN Messages.
 * Writes directly to the DigitalOut pins for the left and right turn signals.
 */
void signalFlashHandler() {
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