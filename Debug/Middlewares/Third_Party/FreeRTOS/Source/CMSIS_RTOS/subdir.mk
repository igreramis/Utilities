################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/%.o: ../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DDEBUG -DSTM32L152xE -DUSE_HAL_DRIVER -DUSE_RTOS_SYSTICK -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Utilities/STM32L1xx_Nucleo" -I"C:/Users/msram/Downloads/STM32CubeExpansion_MEMS1_V5.2.1/Drivers/BSP/Components/Common" -I"C:/Users/msram/Downloads/STM32CubeExpansion_MEMS1_V5.2.1/Drivers/BSP/Components/lsm6dsl" -I"C:/Users/msram/Downloads/STM32CubeExpansion_MEMS1_V5.2.1/Drivers/BSP/IKS01A2" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/HAL_Driver/Inc/Legacy" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/inc" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/CMSIS/device" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/CMSIS/core" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/HAL_Driver/Inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


