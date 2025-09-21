#ifndef POWER_CAN_INTERFACE_H
#define POWER_CAN_INTERFACE_H

#include "CANInterface.h"
#include "ECUCANStructs.h"
#include "MotorControllerCANStructs.h"
#include "DashboardCommandsCANStruct.h"
#include "MotorCommandsCANStruct.h"
#include "AUXBATTERYCANStructs.h"
#include "HeartBeatCANStruct.h" // heartbeat
#include "CANStructs.h"

class PowerCANInterface : public CANInterface {
  public:
    PowerCANInterface(PinName rd, PinName td, PinName standby_pin);
    int send(CANStruct *can_struct);
    int send_message(CANMessage *message);
    
    void handle(BPSError *can_struct);
    void handle(DashboardCommands *can_struct);
    void handle(Heartbeat *can_struct);
    void handle(BPSPackInformation *can_struct);
    void handle(AuxBatteryStatus *can_struct);
    void handle(ChargingMode *can_struct);
  private:
    void message_handler() override;

};



#endif