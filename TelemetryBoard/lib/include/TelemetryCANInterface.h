#ifndef TELEMETRY_CAN_INTERFACE_H
#define TELEMETRY_CAN_INTERFACE_H

#include "BPSCANStructs.h"
#include "CANInterface.h"
#include "ECUCANStructs.h"
#include "MotorControllerCANStructs.h"
#include "DashboardCommandsCANStruct.h"
#include "MotorCommandsCANStruct.h"
#include "CANStructs.h"

class TelemetryCANInterface : public CANInterface {
    public:
        TelemetryCANInterface(PinName rd, PinName td, PinName standby_pin);
        void handle(BPSError *can_struct);
        void handle(DashboardCommands *can_struct);

        int send(CANStruct *can_struct);
        int send_message(CANMessage *message);
        static void send_to_sd(CANMessage *message, uint16_t message_id);
        static void send_to_radio(CANMessage *message, uint16_t message_id);
  
    private:
      void message_handler() override;
  };

#endif