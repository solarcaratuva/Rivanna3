#ifndef POWER_CAN_INTERFACE_H
#define POWER_CAN_INTERFACE_H

#include "BPSCANStructs.h"
#include "CANInterface.h"
#include "ECUCANStructs.h"
#include "MotorControllerCANStructs.h"
#include "DashboardCommandsCANStruct.h"

class PowerCANInterface : public CANInterface {
  public:
    PowerCANInterface(PinName rd, PinName td, PinName standby_pin);
    void handle(DashboardCommands *can_struct);
    int send(CANStruct *can_struct);

  private:
    void message_handler() override;
};

#endif