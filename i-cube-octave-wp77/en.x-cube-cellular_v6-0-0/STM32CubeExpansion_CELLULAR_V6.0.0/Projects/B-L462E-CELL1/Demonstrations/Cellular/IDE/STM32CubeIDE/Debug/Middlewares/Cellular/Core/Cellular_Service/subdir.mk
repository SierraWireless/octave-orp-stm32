################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_cmd.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_config.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_datacache.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_int.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_os.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_power.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_task.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_utils.c 

OBJS += \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_cmd.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_config.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_datacache.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_int.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_os.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_power.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_task.o \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_utils.o 

C_DEPS += \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_cmd.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_config.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_datacache.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_int.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_os.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_power.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_task.d \
./Middlewares/Cellular/Core/Cellular_Service/cellular_service_utils.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Cellular/Core/Cellular_Service/cellular_service.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_cmd.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_cmd.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_cmd.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_config.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_config.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_config.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_datacache.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_datacache.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_datacache.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_int.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_int.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_int.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_os.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_os.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_os.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_power.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_power.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_power.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_task.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_task.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_task.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/Cellular/Core/Cellular_Service/cellular_service_utils.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Src/cellular_service_utils.c Middlewares/Cellular/Core/Cellular_Service/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/Cellular/Core/Cellular_Service/cellular_service_utils.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

