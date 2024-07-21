#ifndef BPS_CAN_INTERFACE_H
#define BPS_CAN_INTERFACE_H

#include "BPSCANStructs.h"
#include "ECUCANStructs.h"
#include "CANInterface.h"

class BPSCANInterface : public CANInterface {
  public:
    BPSCANInterface(PinName rd, PinName td, PinName standby_pin);
    void handle(BPSPackInformation *can_struct);
    void handle(BPSError *can_struct);
    void handle(BPSCellVoltage *can_struct);
    int send(CANStruct *can_struct);
  private:
    void message_handler() override;
    void message_forwarder(CANMessage *message);
};

#endif
