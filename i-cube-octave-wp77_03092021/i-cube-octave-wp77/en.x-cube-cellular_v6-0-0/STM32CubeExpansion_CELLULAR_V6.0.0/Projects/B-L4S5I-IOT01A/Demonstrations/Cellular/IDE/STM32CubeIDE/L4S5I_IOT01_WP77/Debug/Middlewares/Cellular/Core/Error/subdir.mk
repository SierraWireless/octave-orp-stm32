################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
P:/STM/STM32_SWI/i-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Error/Src/error_handler.c 

OBJS += \
./Middlewares/Cellular/Core/Error/error_handler.o 

C_DEPS += \
./Middlewares/Cellular/Core/Error/error_handler.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Cellular/Core/Error/error_handler.o: P:/STM/STM32_SWI/i-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Error/Src/error_handler.c Middlewares/Cellular/Core/Error/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_sensors_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_sensors_thread_config.h"' -DDEBUG -DUSE_COM_MDM -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/WP77/AT_modem_wp77/Inc -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A/Components/Common -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A -I../../../../../../../Misc/Samples/CellularSensors/Inc -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A/Components/hts221 -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A/Components/lis3mdl -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A/Components/lps22hb -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A/Components/lsm6dsl -I../../../../../../../../Middlewares/Third_Party/ORP_Octave/inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Error/error_handler.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

