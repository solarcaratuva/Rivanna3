#include "ECUCANStructs.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>
// Get the DigitalIn pins from Wheelboard
// #include "WheelBoard/include/pindef.h"

#define LOG_LEVEL                    LOG_ERROR
#define MAIN_LOOP_PERIOD             1s
#define MOTOR_LOOP_PERIOD            10ms
#define ERROR_CHECK_PERIOD           100ms
#define FLASH_PERIOD                 500ms
#define IDLE_PERIOD                  100ms
#define THROTTLE_LOW_VOLTAGE         0.66
#define THROTTLE_LOW_VOLTAGE_BUFFER  0.20
#define THROTTLE_HIGH_VOLTAGE        3.08
#define THROTTLE_HIGH_VOLTAGE_BUFFER 0.10
#define UPDATE_SPEED                 5
#define MIN_SPEED                    0
#define MAX_SPEED                    50

// const bool PIN_ON = true;
// const bool PIN_OFF = false;

// bool flashHazards, flashLSignal, flashRSignal = false;
// bool regenEnabled = false;
// bool rpmPositive = false;
// bool strobeEnabled = false;
// bool brakeLightsEnabled = false;
// bool regenActive = false;
bool bms_error = false;
bool contact_12_error = false;
// bool left_turn_state = false;
// bool right_turn_state = true;
// Thread signalFlashThread;
// Thread motor_thread;

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

// Need to get DigitalIn pins from WheelBoard

DigitalIn regen_sda(REGEN_SDA);
DigitalIn regen_scl(REGEN_SCL);

AnalogIn throttle_pedal(THROTTLE_WIPER);
AnalogIn brake_pedal(BRAKE_WIPER);
AnalogIn contactor(CONT_12);
AnalogIn aux_battery(AUX);

// Need to update powercaninterface
PowerCANInterface vehicle_can_interface(UART5_RX, UART5_TX, DEBUG_SWITCH);

// Placeholders for DigitalIn pins
bool leftTurnSignalFlash = false;
bool rightTurnSignalFlash = false;
bool hazardsFlash = false;

void signalFlashHandler() {
    while (true) {
        // Note: Casting from a `DigitalOut` to a `bool` gives the most recently
        // written value
        if (bms_error || contact_12_error) {
            bms_strobe = !bms_strobe;
        }

        // hazards were flashed
        if (hazardsFlash) {
            // if both signals are on, turn them off
            if (left_turn_signal.read() && right_turn_signal.read()) {
                left_turn_signal.write(0);
                right_turn_signal.write(0);
            }
            // otherwise, turn them on (hazards on)
            else {
                left_turn_signal.write(1);
                right_turn_signal.write(1);
            }
        }
        // left turn signal flashed
        else if (leftTurnSignalFlash) {
            // if left signal is on, turn it off
            if (left_turn_signal.read()) {
                left_turn_signal.write(0);
            }
            // otherwise, turn it on and turn right signal off
            else {
                left_turn_signal.write(1);
                right_turn_signal.write(0);
            }
        }
        // right turn signal flashed
        else if (rightTurnSignalFlash) {
            // if right turn signal is on, turn it off
            if (right_turn_signal.read()) {
                right_turn_signal.write(0);
            }
            // otherwise, turn it on and turn left signal off
            else {
                right_turn_signal.write(1);
                left_turn_signal.write(0);
            }
        }

        // Read DigitalIn pins' values directly here and change DigitalOut pins
        // based on them
    }
}