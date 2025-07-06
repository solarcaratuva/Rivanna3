#include "log.h"
#include "AUXBATTERYCANStructs.h"
#include "pindef.h"
#include "PowerCANInterface.h"
#include "main.h"
#include "AUXBattery.h"

#define SCALING_FACTOR          (3.3/3.23)
#define BATTERY_FULL            12.8f
#define BATTERY_EMPTY           11.5f


//read voltage in mV; send to CAN as % full (9V empty, 12V full)
void update_aux_battery() {
    AuxBatteryStatus battery_status = AuxBatteryStatus();

    float voltage = aux_battery.read() * 12.0f * SCALING_FACTOR; //scale to 0–12V initially
    
    //set to mV
    battery_status.aux_voltage = static_cast<uint16_t>(voltage * 1000.0f); 

    float percent_full = (voltage - BATTERY_EMPTY) / (BATTERY_FULL - BATTERY_EMPTY) * 255.0f; 
    if (percent_full < 0) percent_full = 0;
    if (percent_full > 255) percent_full = 255; //hex percent based

    battery_status.percent_full = static_cast<uint8_t>(percent_full);

    vehicle_can_interface.send(&battery_status);
}
