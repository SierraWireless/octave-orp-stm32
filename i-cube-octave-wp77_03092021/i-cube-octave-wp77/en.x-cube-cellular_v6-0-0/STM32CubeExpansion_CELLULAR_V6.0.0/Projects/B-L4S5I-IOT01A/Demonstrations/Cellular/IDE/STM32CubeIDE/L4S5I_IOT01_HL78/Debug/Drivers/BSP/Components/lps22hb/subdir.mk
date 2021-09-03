################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/Components/lps22hb/lps22hb.c 

OBJS += \
./Drivers/BSP/Components/lps22hb/lps22hb.o 

C_DEPS += \
./Drivers/BSP/Components/lps22hb/lps22hb.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/lps22hb/lps22hb.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/Components/lps22hb/lps22hb.c Drivers/BSP/Components/lps22hb/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_iot_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_iot_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Misc/Samples/CellularIoT/Inc -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/HL78/AT_modem_hl78/Inc -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A -I../../../../../../../../Drivers/BSP/B-L4S5I-IOT01A/Components/Common -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/lps22hb/lps22hb.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
