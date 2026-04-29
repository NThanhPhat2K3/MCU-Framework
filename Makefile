# File: Makefile
# Author: Phat Nguyen
# Date: 2026-04-29
# Description: Builds BootManager, Programmer, and App images without IDE dependency.

PROJECT_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

-include $(PROJECT_ROOT)/make/config.mk

TARGET ?= stm32f103
OUT_ROOT ?= $(PROJECT_ROOT)/out
FLASH_TOOL ?= st-flash
FLASH_RESET_FLAGS ?= --reset

CC := /mnt/hdd1/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-gcc
CXX := /mnt/hdd1/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-g++
OBJCOPY := /mnt/hdd1/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objcopy
OBJDUMP := /mnt/hdd1/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-objdump
SIZE := /mnt/hdd1/toolchain/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi/bin/arm-none-eabi-size

COMMON_DIRS := \
	$(PROJECT_ROOT) \
	$(PROJECT_ROOT)/bootloader/common \
	$(PROJECT_ROOT)/port \
	$(PROJECT_ROOT)/startup \
	$(PROJECT_ROOT)/application

STM32F103_DIR := $(PROJECT_ROOT)/stm32f103
STM32F103_CONFIG_DIR := $(STM32F103_DIR)/config
STM32F103_STARTUP_DIR := $(STM32F103_DIR)/startup
STM32F103_SYSTEM_DIR := $(STM32F103_DIR)/system
STM32F103_LD_DIR := $(STM32F103_DIR)/ld

STM32F411CE_DIR := $(PROJECT_ROOT)/stm32f411ce
STM32F411CE_CONFIG_DIR := $(STM32F411CE_DIR)/config
STM32F411CE_STARTUP_DIR := $(STM32F411CE_DIR)/startup
STM32F411CE_SYSTEM_DIR := $(STM32F411CE_DIR)/system
STM32F411CE_LD_DIR := $(STM32F411CE_DIR)/ld

ifeq ($(TARGET),stm32f103)
MCU_FLAGS := -mcpu=cortex-m3 -mthumb
DEFINES := -DSTM32F103xB -DUSE_HAL_DRIVER
INCLUDES := \
	$(addprefix -I,$(COMMON_DIRS)) \
	-I$(STM32F103_CONFIG_DIR) \
	-I$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Inc \
	-I$(STM32CUBE_F1_DIR)/Drivers/CMSIS/Include \
	-I$(STM32CUBE_F1_DIR)/Drivers/CMSIS/Device/ST/STM32F1xx/Include
HAL_SRCS := \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
	$(STM32CUBE_F1_DIR)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
STARTUP_SRCS := \
	$(PROJECT_ROOT)/startup/startup_portable_cortexm.c \
	$(STM32F103_STARTUP_DIR)/startup_stm32f103xb.c \
	$(STM32F103_SYSTEM_DIR)/system_stm32f1xx_min.c
BOOTMANAGER_LD := $(STM32F103_LD_DIR)/bootmanager.ld
PROGRAMMER_LD := $(STM32F103_LD_DIR)/programmer.ld
APP_LD := $(STM32F103_LD_DIR)/app.ld
BOOTMANAGER_ADDR := 0x08000000
PROGRAMMER_ADDR := 0x08004000
APP_ADDR := 0x08008000
CHECK_CONFIG_VAR := STM32CUBE_F1_DIR
CHECK_CONFIG_HINT := STM32CubeF1
else ifeq ($(TARGET),stm32f411ce)
MCU_FLAGS := -mcpu=cortex-m4 -mthumb
DEFINES := -DSTM32F411xE -DUSE_HAL_DRIVER
INCLUDES := \
	$(addprefix -I,$(COMMON_DIRS)) \
	-I$(STM32F411CE_CONFIG_DIR) \
	-I$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc \
	-I$(STM32CUBE_F4_DIR)/Drivers/CMSIS/Include \
	-I$(STM32CUBE_F4_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include
HAL_SRCS := \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
	$(STM32CUBE_F4_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
STARTUP_SRCS := \
	$(PROJECT_ROOT)/startup/startup_portable_cortexm.c \
	$(STM32F411CE_STARTUP_DIR)/startup_stm32f411xe.c \
	$(STM32F411CE_SYSTEM_DIR)/system_stm32f4xx_min.c
BOOTMANAGER_LD := $(STM32F411CE_LD_DIR)/bootmanager.ld
PROGRAMMER_LD := $(STM32F411CE_LD_DIR)/programmer.ld
APP_LD := $(STM32F411CE_LD_DIR)/app.ld
BOOTMANAGER_ADDR := 0x08000000
PROGRAMMER_ADDR := 0x08008000
APP_ADDR := 0x08010000
CHECK_CONFIG_VAR := STM32CUBE_F4_DIR
CHECK_CONFIG_HINT := STM32CubeF4
else
$(error Unsupported TARGET '$(TARGET)')
endif

COMMON_CPPFLAGS := \
	$(MCU_FLAGS) \
	-ffunction-sections \
	-fdata-sections \
	-fno-common \
	-g3 \
	-O0 \
	-Wall \
	-Wextra \
	-MMD \
	-MP \
	$(DEFINES) \
	$(INCLUDES)

COMMON_CFLAGS := \
	$(COMMON_CPPFLAGS) \
	-std=c11

COMMON_CXXFLAGS := \
	$(COMMON_CPPFLAGS) \
	-std=c++17 \
	-fno-exceptions \
	-fno-rtti \
	-fno-threadsafe-statics

COMMON_LDFLAGS := \
	$(MCU_FLAGS) \
	-Wl,--gc-sections \
	-specs=nano.specs \
	-specs=nosys.specs

BOOTMANAGER_SRCS := \
	$(PROJECT_ROOT)/bootloader/bootmanager/main.c \
	$(PROJECT_ROOT)/bootloader/common/boot_jump.c \
	$(PROJECT_ROOT)/bootloader/common/boot_shared.c \
	$(PROJECT_ROOT)/port/port_system.c \
	$(STARTUP_SRCS) \
	$(HAL_SRCS)

PROGRAMMER_SRCS := \
	$(PROJECT_ROOT)/bootloader/programmer/main.c \
	$(PROJECT_ROOT)/bootloader/common/boot_jump.c \
	$(PROJECT_ROOT)/bootloader/common/boot_proto.c \
	$(PROJECT_ROOT)/bootloader/common/boot_shared.c \
	$(PROJECT_ROOT)/port/port_flash.c \
	$(PROJECT_ROOT)/port/port_system.c \
	$(PROJECT_ROOT)/port/port_uart.c \
	$(STARTUP_SRCS) \
	$(HAL_SRCS)

APP_SRCS := \
	$(PROJECT_ROOT)/application/app_main.c \
	$(PROJECT_ROOT)/application/app_update.c \
	$(PROJECT_ROOT)/bootloader/common/boot_shared.c \
	$(PROJECT_ROOT)/port/port_system.c \
	$(STARTUP_SRCS) \
	$(HAL_SRCS)

.PHONY: all help bootmanager programmer app clean print-config check-config \
	bootmanager-image programmer-image app-image \
	flash-bootmanager flash-programmer flash-app flash-all check-flash-tool

all: bootmanager programmer app

help:
	@echo "Portable Boot Example build"
	@echo ""
	@echo "Targets:"
	@echo "  make bootmanager   Build BootManager image"
	@echo "  make programmer    Build Programmer image"
	@echo "  make app           Build App image"
	@echo "  make all           Build all images"
	@echo "  make clean         Remove build output"
	@echo "  make print-config  Print current build configuration"
	@echo "  make flash-bootmanager  Build and flash BootManager via ST-Link"
	@echo "  make flash-programmer   Build and flash Programmer via ST-Link"
	@echo "  make flash-app          Build and flash App via ST-Link"
	@echo "  make flash-all          Build and flash all images via ST-Link"
	@echo ""
	@echo "Configuration:"
	@echo "  1. Copy make/config.mk.example to make/config.mk"
	@echo "  2. Set STM32CUBE_F1_DIR and/or STM32CUBE_F4_DIR to your STM32Cube package path"
	@echo "  3. Install stlink tools so '$(FLASH_TOOL)' is available in PATH"
	@echo ""
	@echo "Examples:"
	@echo "  make TARGET=stm32f103 all"
	@echo "  make TARGET=stm32f411ce all"
	@echo "  make TARGET=stm32f411ce flash-all"
	@echo ""
	@echo "Output:"
	@echo "  out/<target>/<image>/"

print-config:
	@echo "PROJECT_ROOT=$(PROJECT_ROOT)"
	@echo "TARGET=$(TARGET)"
	@echo "STM32CUBE_F1_DIR=$(STM32CUBE_F1_DIR)"
	@echo "STM32CUBE_F4_DIR=$(STM32CUBE_F4_DIR)"
	@echo "FLASH_TOOL=$(FLASH_TOOL)"
	@echo "OUT_ROOT=$(OUT_ROOT)"

check-config:
	@if [ -z "$($(CHECK_CONFIG_VAR))" ]; then \
		echo "$(CHECK_CONFIG_VAR) is not set."; \
		echo "Copy portable_boot_example/make/config.mk.example to portable_boot_example/make/config.mk"; \
		echo "Then set $(CHECK_CONFIG_VAR) to your $(CHECK_CONFIG_HINT) package path."; \
		exit 1; \
	fi

check-flash-tool:
	@if ! command -v $(FLASH_TOOL) >/dev/null 2>&1; then \
		echo "$(FLASH_TOOL) not found in PATH."; \
		echo "Install stlink tools, then run the flash target again."; \
		exit 1; \
	fi

bootmanager: IMAGE := bootmanager
bootmanager: IMAGE_SRCS := $(BOOTMANAGER_SRCS)
bootmanager: IMAGE_LD := $(BOOTMANAGER_LD)
bootmanager: check-config bootmanager-image

programmer: IMAGE := programmer
programmer: IMAGE_SRCS := $(PROGRAMMER_SRCS)
programmer: IMAGE_LD := $(PROGRAMMER_LD)
programmer: check-config programmer-image

app: IMAGE := app
app: IMAGE_SRCS := $(APP_SRCS)
app: IMAGE_LD := $(APP_LD)
app: check-config app-image

flash-bootmanager: bootmanager check-flash-tool
	$(FLASH_TOOL) $(FLASH_RESET_FLAGS) write $(OUT_ROOT)/$(TARGET)/bootmanager/bootmanager.bin $(BOOTMANAGER_ADDR)

flash-programmer: programmer check-flash-tool
	$(FLASH_TOOL) $(FLASH_RESET_FLAGS) write $(OUT_ROOT)/$(TARGET)/programmer/programmer.bin $(PROGRAMMER_ADDR)

flash-app: app check-flash-tool
	$(FLASH_TOOL) $(FLASH_RESET_FLAGS) write $(OUT_ROOT)/$(TARGET)/app/app.bin $(APP_ADDR)

flash-all: all check-flash-tool
	$(FLASH_TOOL) write $(OUT_ROOT)/$(TARGET)/bootmanager/bootmanager.bin $(BOOTMANAGER_ADDR)
	$(FLASH_TOOL) write $(OUT_ROOT)/$(TARGET)/programmer/programmer.bin $(PROGRAMMER_ADDR)
	$(FLASH_TOOL) $(FLASH_RESET_FLAGS) write $(OUT_ROOT)/$(TARGET)/app/app.bin $(APP_ADDR)

bootmanager-image programmer-image app-image:
	@$(MAKE) --no-print-directory \
		IMAGE=$(IMAGE) \
		IMAGE_SRCS='$(IMAGE_SRCS)' \
		IMAGE_LD='$(IMAGE_LD)' \
		image-internal

image-internal:
	@mkdir -p $(OUT_ROOT)/$(TARGET)/$(IMAGE)
	@$(MAKE) --no-print-directory \
		IMAGE=$(IMAGE) \
		IMAGE_SRCS='$(IMAGE_SRCS)' \
		IMAGE_LD='$(IMAGE_LD)' \
		IMAGE_OUT=$(OUT_ROOT)/$(TARGET)/$(IMAGE) \
		image-link

image-link: $(IMAGE_OUT)/$(IMAGE).elf $(IMAGE_OUT)/$(IMAGE).bin $(IMAGE_OUT)/$(IMAGE).hex $(IMAGE_OUT)/$(IMAGE).lst
	@$(SIZE) $(IMAGE_OUT)/$(IMAGE).elf

IMAGE_OBJS = \
	$(patsubst $(PROJECT_ROOT)/%.c,$(IMAGE_OUT)/obj/%.o,$(filter %.c,$(IMAGE_SRCS))) \
	$(patsubst $(PROJECT_ROOT)/%.cpp,$(IMAGE_OUT)/obj/%.o,$(filter %.cpp,$(IMAGE_SRCS)))
IMAGE_DEPS = $(IMAGE_OBJS:.o=.d)

$(IMAGE_OUT)/$(IMAGE).elf: $(IMAGE_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_LDFLAGS) -T$(IMAGE_LD) -Wl,-Map,$(IMAGE_OUT)/$(IMAGE).map -o $@ $^

$(IMAGE_OUT)/$(IMAGE).bin: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJCOPY) -O binary $< $@

$(IMAGE_OUT)/$(IMAGE).hex: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJCOPY) -O ihex $< $@

$(IMAGE_OUT)/$(IMAGE).lst: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJDUMP) -h -S $< > $@

$(IMAGE_OUT)/obj/%.o: $(PROJECT_ROOT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_CFLAGS) -c $< -o $@

$(IMAGE_OUT)/obj/%.o: $(PROJECT_ROOT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_CXXFLAGS) -c $< -o $@

-include $(IMAGE_DEPS)

clean:
	rm -rf $(OUT_ROOT)
