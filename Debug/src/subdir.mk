################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/app_mems-library.c \
../src/app_mems_int_pin_a_interface.c \
../src/app_uart_interface.c \
../src/flash.c \
../src/iks01a2_motion_sensors.c \
../src/iks01a2_motion_sensors_ex.c \
../src/lsm6dsl.c \
../src/lsm6dsl_reg.c \
../src/main.c \
../src/nucleo_l152re_bus.c \
../src/stm32_common_bsp.c \
../src/stm32l1xx_hal_exti.c \
../src/stm32l1xx_hal_msp.c \
../src/stm32l1xx_it.c \
../src/syscalls.c \
../src/system_stm32l1xx.c 

OBJS += \
./src/app_mems-library.o \
./src/app_mems_int_pin_a_interface.o \
./src/app_uart_interface.o \
./src/flash.o \
./src/iks01a2_motion_sensors.o \
./src/iks01a2_motion_sensors_ex.o \
./src/lsm6dsl.o \
./src/lsm6dsl_reg.o \
./src/main.o \
./src/nucleo_l152re_bus.o \
./src/stm32_common_bsp.o \
./src/stm32l1xx_hal_exti.o \
./src/stm32l1xx_hal_msp.o \
./src/stm32l1xx_it.o \
./src/syscalls.o \
./src/system_stm32l1xx.o 

C_DEPS += \
./src/app_mems-library.d \
./src/app_mems_int_pin_a_interface.d \
./src/app_uart_interface.d \
./src/flash.d \
./src/iks01a2_motion_sensors.d \
./src/iks01a2_motion_sensors_ex.d \
./src/lsm6dsl.d \
./src/lsm6dsl_reg.d \
./src/main.d \
./src/nucleo_l152re_bus.d \
./src/stm32_common_bsp.d \
./src/stm32l1xx_hal_exti.d \
./src/stm32l1xx_hal_msp.d \
./src/stm32l1xx_it.d \
./src/syscalls.d \
./src/system_stm32l1xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32L1 -DSTM32L152RETx -DNUCLEO_L152RE -DDEBUG -DSTM32L152xE -DUSE_HAL_DRIVER -DUSE_RTOS_SYSTICK -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Utilities/STM32L1xx_Nucleo" -I"C:/Users/msram/Downloads/STM32CubeExpansion_MEMS1_V5.2.1/Drivers/BSP/Components/Common" -I"C:/Users/msram/Downloads/STM32CubeExpansion_MEMS1_V5.2.1/Drivers/BSP/Components/lsm6dsl" -I"C:/Users/msram/Downloads/STM32CubeExpansion_MEMS1_V5.2.1/Drivers/BSP/IKS01A2" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/HAL_Driver/Inc/Legacy" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/inc" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/CMSIS/device" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/CMSIS/core" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/msram/Downloads/workspace_swb/freertos_proj4/HAL_Driver/Inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


