################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Misc/Samples/Cellular/subdir.mk
-include Misc/Freertos/subdir.mk
-include Misc/Cmd/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/CMSIS_RTOS/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/subdir.mk
-include Middlewares/Cellular/Interface/Control/subdir.mk
-include Middlewares/Cellular/Interface/Com/subdir.mk
-include Middlewares/Cellular/Core/Trace/subdir.mk
-include Middlewares/Cellular/Core/Runtime_Library/subdir.mk
-include Middlewares/Cellular/Core/Rtosal/subdir.mk
-include Middlewares/Cellular/Core/PPPosif/subdir.mk
-include Middlewares/Cellular/Core/Ipc/subdir.mk
-include Middlewares/Cellular/Core/HW_Specific/subdir.mk
-include Middlewares/Cellular/Core/Error/subdir.mk
-include Middlewares/Cellular/Core/Data_Cache/subdir.mk
-include Middlewares/Cellular/Core/Cellular_Service/subdir.mk
-include Middlewares/Cellular/Core/AT_Core/subdir.mk
-include Drivers/STM32L4xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/X_STMOD_PLUS_MODEMS/TYPE1SC/subdir.mk
-include Application/User/subdir.mk
-include Application/Startup/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := DiscoL462_T1SC
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
DiscoL462_T1SC.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
DiscoL462_T1SC.list \

OBJCOPY_BIN += \
DiscoL462_T1SC.bin \


# All Target
all: main-build

# Main-build Target
main-build: DiscoL462_T1SC.elf secondary-outputs

# Tool invocations
DiscoL462_T1SC.elf: $(OBJS) $(USER_OBJS) P:\STM\STM32_SWI\x-cube-octave-wp77\en.x-cube-cellular_v6-0-0\STM32CubeExpansion_CELLULAR_V6.0.0\Projects\B-L462E-CELL1\Demonstrations\Cellular\IDE\STM32CubeIDE\STM32L462RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "DiscoL462_T1SC.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"P:\STM\STM32_SWI\x-cube-octave-wp77\en.x-cube-cellular_v6-0-0\STM32CubeExpansion_CELLULAR_V6.0.0\Projects\B-L462E-CELL1\Demonstrations\Cellular\IDE\STM32CubeIDE\STM32L462RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="DiscoL462_T1SC.map" -Wl,--gc-sections -static -u_printf_float  -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

DiscoL462_T1SC.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "DiscoL462_T1SC.list"
	@echo 'Finished building: $@'
	@echo ' '

DiscoL462_T1SC.bin: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "DiscoL462_T1SC.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
