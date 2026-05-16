# Toolchain
CC      = arm-none-eabi-gcc
AS      = arm-none-eabi-as
LD      = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# CPU flags
CPU     = -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# Files
TARGET  = blink
SRCS    = src/main.c Driver/Clock/Clock.c
STARTUP = startup/startup_stm32f407.s
LINKER  = linker/stm32f407.ld

# Build
all:
	$(CC) $(CPU) -g -nostdlib -T$(LINKER) $(STARTUP) $(SRCS) -o $(TARGET).elf
	$(OBJCOPY) -O binary $(TARGET).elf $(TARGET).bin
	$(OBJCOPY) -O ihex   $(TARGET).elf $(TARGET).hex

# Flash using OpenOCD
flash:
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
	-c "program $(TARGET).elf verify reset exit"

# Clean
clean:
	del $(TARGET).elf $(TARGET).bin $(TARGET).hex