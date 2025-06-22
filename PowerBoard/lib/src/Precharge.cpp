#include <mbed.h>
#include <rtos.h>
#include "log.h"
#include "main.h"


#define PRECHARGE_CHARGING          2500ms
#define SLEEP_TICK                  10ms
#define CONT_12_LOGIC_THRESHOLD     0.1f // below is logic low, above is logic high
#define HAL_EFFECT_SENSOR_SCALE     (5.0f / 3.28f) // used to scale voltages up to 5V
#define PACK_VOLT_PCT               0.95f // precharge can proceed once RC voltage exceeds this percent of pack voltage
#define PRECHARGE_WAIT              3s

#define PERCENT                     0.9 
#define SENSITIVITY                 0.8
#define VCC                         5
#define RESISTANCE                  74


// checks if a fault occured
// if so, it will disable the precharge relays and wait forever
void fault_trap(){
    if (has_faulted || cont_12.read() < CONT_12_LOGIC_THRESHOLD) {
        discharge_en.write(0);
        motor_precharge_en.write(0);
        while(true) {
            ThisThread::sleep_for(0xFFFFFFFFms); // wait forever
        }
    }
}


// starts and runs motor precharge, should be run in a thread, runs forever
// void motor_precharge(){
//     log_debug("Motor precharge starting");

//     while (cont_12.read() < CONT_12_LOGIC_THRESHOLD || !discharge_relay_status){
//         ThisThread::sleep_for(SLEEP_TICK);
//     }

//     fault_trap();
//     // start motor precharge
//     motor_precharge_en.write(1);

//     // invert rc voltage
//     // while (pack_voltage == 0 && rc_voltage_motor.read_voltage() * HAL_EFFECT_SENSOR_SCALE < PACK_VOLT_PCT * pack_voltage) {
//     //     ThisThread::sleep_for(SLEEP_TICK);
//     //     fault_trap();
//     // }

//     ThisThread::sleep_for(PRECHARGE_WAIT);

//     discharge_en.write(1);

//     uint32_t count = PRECHARGE_CHARGING / SLEEP_TICK;
//     for (int i = 0; i < count; i++) {
//         ThisThread::sleep_for(SLEEP_TICK);
//         fault_trap();
//     }

//     // end motor precharge
//     motor_precharge_en.write(0);
//     log_debug("Finished motor precharge");

//     // will wait here forever waiting for a fault
//     while (true) {
//         ThisThread::sleep_for(SLEEP_TICK);
//         fault_trap();
//     }
// }

void motor_precharge() {
    log_error("PRECHARGE Motor precharge starting");

    motor_precharge_en.write(1);

    // wait for pack voltage to be set (non 0 value)
    while (pack_voltage == 0)
    {
        ThisThread::sleep_for(SLEEP_TICK);
    }

    log_error("PRECHARGE Pack voltage set: %d", (int) (1000 * pack_voltage));

    // calculate the threshold voltage for precharge being done
    float threshold =(((pack_voltage - pack_voltage * PERCENT) / RESISTANCE) * SENSITIVITY + (VCC * 0.1)) * (3.28 / VCC);

    // wait for RCvoltage to be below the threshold
    int i = 0;

    while (true) {
        if (rc_voltage_motor.read_voltage() < threshold) {
            i += 1;
        }
        else {
            i = 0;
        }

        log_debug("PRECHARGE RC voltage: %d, threshold: %d, i: %d, pack voltage: %d", (int) (1000 * rc_voltage_motor.read_voltage()), (int) (1000 * threshold), i, (int) (pack_voltage * 1000));

        if (i == 5) { // if the voltage is below the threshold for 2 iterations, we can proceed
            break;
        }

        ThisThread::sleep_for(30ms);
    }

    discharge_en.write(1);

    log_debug("PRECHARGE Discharge relay enabled");
    ThisThread::sleep_for(SLEEP_TICK);

    // end motor precharge
    motor_precharge_en.write(0);
    log_debug("PRECHARGE Motor precharge ended");
    // log_debug("Finished motor precharge");

    // will wait here forever waiting for a fault
    while (true) {
        ThisThread::sleep_for(SLEEP_TICK);
    }


}


// starts and runs MPPT / battery precharge, should be run in a thread, runs forever
void mppt_precharge(){
    log_debug("MPPT precharge starting");

    while(cont_12.read() < CONT_12_LOGIC_THRESHOLD || !charge_relay_status){
        ThisThread::sleep_for(SLEEP_TICK);
    }

    fault_trap();
    // start motor precharge
    mppt_precharge_en.write(1);

    // while (pack_voltage == 0 && rc_voltage_battery.read_voltage() * HAL_EFFECT_SENSOR_SCALE < PACK_VOLT_PCT * pack_voltage) {
    //     ThisThread::sleep_for(SLEEP_TICK);
    //     fault_trap();
    // }

    ThisThread::sleep_for(PRECHARGE_WAIT);

    charge_en.write(1);

    uint32_t count = PRECHARGE_CHARGING / SLEEP_TICK;
    for (int i = 0; i < count; i++) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }

    // end motor precharge
    mppt_precharge_en.write(0);
    log_debug("Finished MPPT precharge");

    // will wait here forever waiting for a fault
    while (true) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }
}

