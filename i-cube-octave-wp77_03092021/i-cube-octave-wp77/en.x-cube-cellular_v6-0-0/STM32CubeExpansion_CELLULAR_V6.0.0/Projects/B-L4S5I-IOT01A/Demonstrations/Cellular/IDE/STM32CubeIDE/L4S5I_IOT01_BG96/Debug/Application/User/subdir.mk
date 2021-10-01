################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/gpio.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/main.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/rng.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/rtc.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/stm32l4xx_hal_msp.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/stm32l4xx_hal_timebase_tim.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/stm32l4xx_it.c \
../Application/User/syscalls.c \
../Application/User/sysmem.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/usart.c 

OBJS += \
./Application/User/gpio.o \
./Application/User/main.o \
./Application/User/rng.o \
./Application/User/rtc.o \
./Application/User/stm32l4xx_hal_msp.o \
./Application/User/stm32l4xx_hal_timebase_tim.o \
./Application/User/stm32l4xx_it.o \
./Application/User/syscalls.o \
./Application/User/sysmem.o \
./Application/User/usart.o 

C_DEPS += \
./Application/User/gpio.d \
./Application/User/main.d \
./Application/User/rng.d \
./Application/User/rtc.d \
./Application/User/stm32l4xx_hal_msp.d \
./Application/User/stm32l4xx_hal_timebase_tim.d \
./Application/User/stm32l4xx_it.d \
./Application/User/syscalls.d \
./Application/User/sysmem.d \
./Application/User/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/gpio.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/gpio.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/gpio.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/main.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/main.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/main.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/rng.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/rng.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/rng.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/rtc.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/rtc.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/rtc.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32l4xx_hal_msp.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/stm32l4xx_hal_msp.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/stm32l4xx_hal_msp.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32l4xx_hal_timebase_tim.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/stm32l4xx_hal_timebase_tim.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/stm32l4xx_hal_timebase_tim.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/stm32l4xx_it.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/stm32l4xx_it.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/stm32l4xx_it.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/syscalls.o: ../Application/User/syscalls.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/syscalls.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/sysmem.o: ../Application/User/sysmem.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/sysmem.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Application/User/usart.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Projects/B-L4S5I-IOT01A/Demonstrations/Cellular/Core/Src/usart.c Application/User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L4S5xx -DHAS_RTOS -DUSER_FLAG_MODEM_FORCE_NO_FLOW_CTRL -DHWREF_B_CELL_BG96_V2 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../../Core/Inc -I../../../../STM32_Cellular/Config -I../../../../STM32_Cellular/Target -I../../../../../../../Misc/Cmd/Inc -I../../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../../Drivers/CMSIS/Include -I../../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/BG96/AT_modem_bg96/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../../Middlewares/Third_Party/LwIP/system -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Application/User/usart.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

