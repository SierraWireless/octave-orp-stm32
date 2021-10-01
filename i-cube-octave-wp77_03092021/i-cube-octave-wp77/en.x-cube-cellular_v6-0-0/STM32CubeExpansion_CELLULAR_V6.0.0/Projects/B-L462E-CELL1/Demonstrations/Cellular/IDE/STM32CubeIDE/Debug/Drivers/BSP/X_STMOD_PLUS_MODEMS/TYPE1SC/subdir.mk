################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_api.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_signalling.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_socket.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_specific.c \
P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/sysctrl_specific.c 

OBJS += \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_api.o \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_signalling.o \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_socket.o \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_specific.o \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/sysctrl_specific.o 

C_DEPS += \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_api.d \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_signalling.d \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_socket.d \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_specific.d \
./Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/sysctrl_specific.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_api.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_api.c Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_api.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_signalling.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_signalling.c Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_signalling.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_socket.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_socket.c Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_socket.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_specific.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/at_custom_modem_specific.c Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/at_custom_modem_specific.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/sysctrl_specific.o: P:/STM/STM32_SWI/x-cube-octave-wp77/en.x-cube-cellular_v6-0-0/STM32CubeExpansion_CELLULAR_V6.0.0/Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Src/sysctrl_specific.c Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32L462xx -DUSE_STM32L462E_CELL01 -DDISCO_L462 '-DAPPLICATION_CONFIG_FILE="plf_cellular_app_config.h"' '-DAPPLICATION_THREAD_CONFIG_FILE="plf_cellular_app_thread_config.h"' -DDEBUG -c -I../../../Core/Inc -I../../../STM32_Cellular/Config -I../../../STM32_Cellular/Target -I../../../../../../Misc/Cmd/Inc -I../../../../../../Misc/RTOS/FreeRTOS/Inc -I../../../../../../Misc/Samples/Cellular/Inc -I../../../../../../../Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/AT_modem_type1sc/Inc -I../../../../../../../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../../../../../../../Drivers/CMSIS/Include -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc -I../../../../../../../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../../../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../../../../../Middlewares/Third_Party/LwIP/system/arch -I../../../../../../../Middlewares/Third_Party/LwIP/src/include -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/lwip/prot -I../../../../../../../Middlewares/Third_Party/LwIP/src/include/netif/ppp -I../../../../../../../Middlewares/Third_Party/LwIP/system -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/AT_Core/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Cellular_Service/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Data_Cache/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Error/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Ipc/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/PPPosif/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Runtime_Library/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Rtosal/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Core/Trace/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Cellular_Ctrl/Inc -I../../../../../../../Middlewares/ST/STM32_Cellular/Interface/Com/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/sysctrl_specific.d" -MT"$@"  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

