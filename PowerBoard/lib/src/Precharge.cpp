#include <mbed.h>
#include <rtos.h>
#include "log.h"
#include "main.h"


#define SLEEP_TICK                  10ms
#define CONT_12_LOGIC_THRESHOLD     0.1f // below is logic low, above is logic high
#define PACK_VOLT_PCT               0.9 
#define HAL_EFFECT_SENSITIVITY      0.8
#define VCC                         5
#define PRECHARGE_RESISTANCE        74
#define WAITING_PERIOD              50ms


// checks if a fault occured
// if so, it will disable the precharge relays and wait forever
void fault_trap(){
    if (has_faulted || cont_12.read() < CONT_12_LOGIC_THRESHOLD) {
        discharge_en.write(0);
        charge_en.write(0);
        motor_precharge_en.write(0);
        mppt_precharge_en.write(0);
        log_debug("fault in precharge");
        while(true) {
            ThisThread::sleep_for(0xFFFFFFFFms); // wait forever
        }
    }
}

// starts and runs motor precharge, should be run in a thread, runs forever
void motor_precharge() {
    log_debug("Motor precharge thread starting");

    discharge_en.write(0);
    motor_precharge_en.write(1);

    // wait for pack voltage to be set (non 0 value) and contactor 12 to be high
    while (pack_voltage == 0 || cont_12.read() < CONT_12_LOGIC_THRESHOLD)
    {
        ThisThread::sleep_for(SLEEP_TICK);
    }

    log_debug("Motor precharge now waiting on the HAL effect sensor");

    // calculate the threshold voltage for precharge being done
    float threshold =(((pack_voltage - pack_voltage * PACK_VOLT_PCT) / PRECHARGE_RESISTANCE) * HAL_EFFECT_SENSITIVITY + (VCC * 0.1)) * (3.28 / VCC);

    // wait for HAL Effect Sensor voltage to be below the threshold
    int i = 0;
    while (true) {
        if (hal_effect_voltage_motor.read_voltage() < threshold) {
            i += 1;
        }
        else {
            i = 0;
        }

        if (i == WAITING_PERIOD / SLEEP_TICK) { // if the HAL Effect Sensor voltage is below the threshold for a few iterations, we can proceed
            break;
        }

        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }
    log_debug("Motor precharge finished waiting on the HAL effect sensor");

    discharge_en.write(1);
    ThisThread::sleep_for(30ms);
    // end motor precharge
    motor_precharge_en.write(0);

    log_debug("Motor precharge ended");

    // will wait here forever waiting for a fault
    while (true) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }
}


// starts and runs MPPT / battery precharge, should be run in a thread, runs forever
void mppt_precharge(){
    log_debug("MPPT precharge thread starting");

    charge_en.write(0);
    mppt_precharge_en.write(1);

    // wait for pack voltage to be set (non 0 value) and contactor 12 to be high
    while (pack_voltage == 0 || cont_12.read() < CONT_12_LOGIC_THRESHOLD)
    {
        ThisThread::sleep_for(SLEEP_TICK);
    }

    log_debug("MPPT precharge now waiting on the HAL effect sensor");

    // calculate the threshold voltage for precharge being done
    float threshold =(((pack_voltage - pack_voltage * PACK_VOLT_PCT) / PRECHARGE_RESISTANCE) * HAL_EFFECT_SENSITIVITY + (VCC * 0.1)) * (3.28 / VCC);

    // wait for HAL Effect Sensor voltage to be below the threshold
    int i = 0;
    while (true) {
        if (hal_effect_voltage_mppt.read_voltage() < threshold) {
            i += 1;
        }
        else {
            i = 0;
        }

        if (i == WAITING_PERIOD / SLEEP_TICK) { // if the HAL Effect Sensor voltage is below the threshold for a few iterations, we can proceed
            break;
        }

        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }
    log_debug("MPPT precharge finished waiting on the HAL effect sensor");

    charge_en.write(1);
    ThisThread::sleep_for(30ms);
    // end MPPT precharge
    mppt_precharge_en.write(0);

    log_debug("MPPT precharge ended");

    // will wait here forever waiting for a fault
    while (true) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }
}