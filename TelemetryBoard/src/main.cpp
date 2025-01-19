#include "TelemetryCANInterface.h"
#include "ThisThread.h"
#include "log.h"
#include <mbed.h>

#define placeholderRD 
#define placeholderTD 
#define placeholderSTANDBY 

TelemetryCANInterface telemetry(placeholderRD, placeholderTD, placeholderSTANDBY); 

void TelemetryCANInterface::handle(ECUPowerAuxCommands *can_struct) {
    
}

void TelemetryCANInterface::handle(PowerAuxError *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarCurrent *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarTemp *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarVoltage *can_struct) {
    
}

void TelemetryCANInterface::handle(SolarPhoto *can_struct) {
    
}

void TelemetryCANInterface::handle(MotorControllerPowerStatus *can_struct) {
    
}

void TelemetryCANInterface::handle(MotorControllerDriveStatus *can_struct) {
    
}

void TelemetryCANInterface::handle(MotorControllerError *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSPackInformation *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSError *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSCellVoltage *can_struct) {
    
}

void TelemetryCANInterface::handle(BPSCellTemperature *can_struct) {
    
}