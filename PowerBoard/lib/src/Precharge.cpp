#include <mbed.h>
#include <rtos.h>
#include "log.h"
#include "main.h"


#define PRECHARGE_CHARGING      2500ms
#define SLEEP_TICK              10ms
#define CONT_12_LOGIC_THRESHOLD 0.1f // below is logic low, above is logic high


// checks if a fault occured
// if so, it will disable the precharge relays and wait forever
void fault_trap(){
    if (has_faulted || cont_12.read_voltage() < CONT_12_LOGIC_THRESHOLD) {
        discharge_en.write(0);
        motor_precharge_en.write(0);
        ThisThread::sleep_for(0xFFFFFFFFms); // wait forever
    }
}


// starts and runs motor precharge, should be run in a thread, runs forever
void motor_precharge(){
    log_debug("Motor precharge starting");

    while (cont_12.read_voltage() < CONT_12_LOGIC_THRESHOLD || !discharge_relay_status){
        ThisThread::sleep_for(SLEEP_TICK);
    }

    fault_trap();
    // start motor precharge
    motor_precharge_en.write(1);

    while (rc_voltage_motor.read_voltage() < 0.95 * pack_voltage) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }

    discharge_en.write(1);

    uint32_t count = PRECHARGE_CHARGING / SLEEP_TICK;
    for (int i = 0; i < count; i++) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }

    // end motor precharge
    motor_precharge_en.write(0);
    log_debug("Finished motor precharge");

    // will wait here forever waiting for a fault
    while (true) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }
}


// starts and runs MPPT / battery precharge, should be run in a thread, runs forever
void mppt_precharge(){
    log_debug("MPPT precharge starting");

    while(cont_12.read_voltage() < CONT_12_LOGIC_THRESHOLD || !charge_relay_status){
        ThisThread::sleep_for(SLEEP_TICK);
    }

    fault_trap();
    // start motor precharge
    mppt_precharge_en.write(1);

    while (rc_voltage_battery.read_voltage() < 0.95 * pack_voltage) {
        ThisThread::sleep_for(SLEEP_TICK);
        fault_trap();
    }

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

