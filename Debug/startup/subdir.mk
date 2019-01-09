################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32l152xe.s 

OBJS += \
./startup/startup_stm32l152xe.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Utilities/STM32L1xx_Nucleo" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/HAL_Driver/Inc/Legacy" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/inc" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/CMSIS/device" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/CMSIS/core" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/HAL_Driver/Inc" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


