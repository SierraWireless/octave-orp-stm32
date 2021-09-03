################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Application/Startup/startup_stm32l4s5vitx.s 

OBJS += \
./Application/Startup/startup_stm32l4s5vitx.o 

S_DEPS += \
./Application/Startup/startup_stm32l4s5vitx.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Startup/startup_stm32l4s5vitx.o: ../Application/Startup/startup_stm32l4s5vitx.s Application/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -x assembler-with-cpp -MMD -MP -MF"Application/Startup/startup_stm32l4s5vitx.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

