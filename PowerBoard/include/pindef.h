#ifndef PINDEF_H
#define PINDEF_H

#ifdef TARGET_NUCLEO_F413ZH // if compiling for PowerBoard

// HiL pins
#define CRUISE_ENABLED PB_14
#define CRUISE_INC PB_13
#define CRUISE_DEC PA_7


#define LEFT_TURN_IN PC_11
#define RIGHT_TURN_IN PA_9
#define HAZARDS_IN PB_0
#define MECHANICAL_BRAKE_IN PA_8
#define REGEN_IN PB_11
#define THROTTLE_VALUE_IN PC_3
#define FORWARD_REVERSE_IN NC  // only one not done

#define LEFT_TURN_OUT PC_10
#define RIGHT_TURN_OUT PA_1
#define BRAKE_LIGHTS_OUT PA_4

#define DRL_OUT PA_0
#define BMS_STROBE_OUT PA_3

#define CAN_RX PA_11
#define CAN_TX PA_12
#define CAN_STBY PB_10

#define LED2_CTR PB_7

#define I2C_SDA1 PB_11
#define I2C_SCL1 PB_10


//NON HiL PINS; DID NOT MERGE WITH PINS ABOVE 
#define DEBUG_LED_1         PE_11
// #define DEBUG_LED_2         PE_15
#define STROBE_EN           PF_13
#define BRAKE_LIGHT_EN      PF_12
#define RIGHT_TURN_EN       PD_15
#define LEFT_TURN_EN        PD_14
#define DRL_EN              PA_7

#define THROTTLE_WIPER      PA_6
#define BRAKE_WIPER         PA_5

#define AUX                 PC_3
#define CONT_12             PC_1
#define MPPT_HAL_SENSE      PC_4
#define MTR_HAL_SENSE       PC_5

#define DIS_CHARGE_EN       PF_2
#define MTR_PRE_EN          PF_1
#define CHARGE_EN           PF_0
#define MPPT_PRE_EN         PD_0

#define MTR_SDA             PB_11
#define MTR_SCL             PB_10

#define MTR_CAN_RX          PG_0
#define MTR_CAN_TX          PD_1

#define MAIN_CAN_STBY       NC
#define MAIN_CAN_RX         PB_12
#define MAIN_CAN_TX         PB_13


#else // if not compiling for PowerBoard

#define DEBUG_BTN       NC
#define DEBUG_LED_1     NC
#define DEBUG_LED_2     NC
#define STROBE_EN       NC
#define BRAKE_LIGHT_EN  NC
#define RIGHT_TURN_EN   NC
#define LEFT_TURN_EN    NC
#define DRL_EN          NC

#define THROTTLE_WIPER  NC
#define BRAKE_WIPER     NC

#define AUX             NC
#define CONT_12         NC
#define MPPT_HAL_SENSE  NC
#define MTR_HAL_SENSE   NC

#define DIS_CHARGE_EN   NC
#define MTR_PRE_EN      NC
#define CHARGE_EN       NC
#define MPPT_PRE_EN     NC

#define MTR_SDA         NC
#define MTR_SCL         NC

#define MTR_CAN_RX      NC
#define MTR_CAN_TX      NC

#define MAIN_CAN_STBY   NC
#define MAIN_CAN_RX     NC
#define MAIN_CAN_TX     NC

#define PACK_VOLTAGE    NC

#define DEBUG_SWITCH    NC

#endif
#endif


