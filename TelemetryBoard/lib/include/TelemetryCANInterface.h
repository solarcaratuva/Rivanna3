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
        void handle(PowerAuxError *can_struct);
        void handle(MotorControllerError *can_struct);
        void handle(BPSError *can_struct);
        void handle(ECUMotorCommands *can_struct);
        void handle(MotorControllerDriveStatus *can_struct);
        void handle(SolarCurrent *can_struct);
        void handle(SolarTemp *can_struct);
        void handle(SolarVoltage *can_struct);
        void handle(SolarPhoto *can_struct);
        void handle(BPSPackInformation *can_struct);
        void handle(BPSCellVoltage *can_struct);
        void handle(BPSCellTemperature *can_struct);
        void handle(MotorControllerPowerStatus *can_struct);
        void handle(ECUPowerAuxCommands *can_struct);


        int send(CANStruct *can_struct);
        int send_message(CANMessage *message);
        static void send_to_sd(CANMessage *message, uint16_t message_id);
        static void send_to_radio(CANMessage *message, uint16_t message_id);
  
    private:
      void message_handler() override;
  };

#endif