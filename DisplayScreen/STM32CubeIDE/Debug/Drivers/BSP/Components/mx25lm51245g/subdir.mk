################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/Ethan/Documents/SolarCar/Rivanna3/DisplayScreen/Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.c 

C_DEPS += \
./Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.d 

OBJS += \
./Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.o: C:/Users/Ethan/Documents/SolarCar/Rivanna3/DisplayScreen/Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.c Drivers/BSP/Components/mx25lm51245g/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32U5A9xx -c -I../../Core/Inc -I../../TouchGFX/App -I../../TouchGFX/target/generated -I../../TouchGFX/target -I../../Drivers/STM32U5xx_HAL_Driver/Inc -I../../Drivers/STM32U5xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32U5xx/Include -I../../Drivers/CMSIS/Include -I../../Drivers/BSP/Components/mx25lm51245g -I../../Middlewares/Third_Party/FreeRTOS/Source/include/ -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM33_NTZ/non_secure/ -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/ -I../../Middlewares/Third_Party/CMSIS/RTOS2/Include/ -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/generated/videos/include -I../../TouchGFX/gui/include -I../../Middlewares/ST/touchgfx_components/gpu2d/NemaGFX/include -I../../Middlewares/ST/touchgfx_components/gpu2d/TouchGFXNema/include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-mx25lm51245g

clean-Drivers-2f-BSP-2f-Components-2f-mx25lm51245g:
	-$(RM) ./Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.cyclo ./Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.d ./Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.o ./Drivers/BSP/Components/mx25lm51245g/mx25lm51245g.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-mx25lm51245g

