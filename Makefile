PROJECT_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

-include $(PROJECT_ROOT)/make/config.mk

TARGET ?= stm32f103
OUT_ROOT ?= $(PROJECT_ROOT)/out

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
OBJDUMP := arm-none-eabi-objdump
SIZE := arm-none-eabi-size

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
else
$(error Unsupported TARGET '$(TARGET)')
endif

COMMON_CFLAGS := \
	$(MCU_FLAGS) \
	-std=c11 \
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

BOOTMANAGER_LD := $(STM32F103_LD_DIR)/bootmanager.ld
PROGRAMMER_LD := $(STM32F103_LD_DIR)/programmer.ld
APP_LD := $(STM32F103_LD_DIR)/app.ld

.PHONY: all help bootmanager programmer app clean print-config check-config

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
	@echo ""
	@echo "Configuration:"
	@echo "  1. Copy make/config.mk.example to make/config.mk"
	@echo "  2. Set STM32CUBE_F1_DIR to your STM32CubeF1 package path"
	@echo ""
	@echo "Output:"
	@echo "  out/stm32f103/<image>/"

print-config:
	@echo "PROJECT_ROOT=$(PROJECT_ROOT)"
	@echo "TARGET=$(TARGET)"
	@echo "STM32CUBE_F1_DIR=$(STM32CUBE_F1_DIR)"
	@echo "OUT_ROOT=$(OUT_ROOT)"

check-config:
	@if [ -z "$(STM32CUBE_F1_DIR)" ]; then \
		echo "STM32CUBE_F1_DIR is not set."; \
		echo "Copy portable_boot_example/make/config.mk.example to portable_boot_example/make/config.mk"; \
		echo "Then set STM32CUBE_F1_DIR to your STM32CubeF1 package path."; \
		exit 1; \
	fi

bootmanager: IMAGE := bootmanager
bootmanager: IMAGE_SRCS := $(BOOTMANAGER_SRCS)
bootmanager: IMAGE_LD := $(BOOTMANAGER_LD)
bootmanager: check-config build-image

programmer: IMAGE := programmer
programmer: IMAGE_SRCS := $(PROGRAMMER_SRCS)
programmer: IMAGE_LD := $(PROGRAMMER_LD)
programmer: check-config build-image

app: IMAGE := app
app: IMAGE_SRCS := $(APP_SRCS)
app: IMAGE_LD := $(APP_LD)
app: check-config build-image

build-image:
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

IMAGE_OBJS = $(patsubst $(PROJECT_ROOT)/%.c,$(IMAGE_OUT)/obj/%.o,$(filter %.c,$(IMAGE_SRCS)))
IMAGE_DEPS = $(IMAGE_OBJS:.o=.d)

$(IMAGE_OUT)/$(IMAGE).elf: $(IMAGE_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_LDFLAGS) -T$(IMAGE_LD) -Wl,-Map,$(IMAGE_OUT)/$(IMAGE).map -o $@ $^

$(IMAGE_OUT)/$(IMAGE).bin: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJCOPY) -O binary $< $@

$(IMAGE_OUT)/$(IMAGE).hex: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJCOPY) -O ihex $< $@

$(IMAGE_OUT)/$(IMAGE).lst: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJDUMP) -h -S $< > $@

$(IMAGE_OUT)/obj/%.o: $(PROJECT_ROOT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(COMMON_CFLAGS) -c $< -o $@

-include $(IMAGE_DEPS)

clean:
	rm -rf $(OUT_ROOT)
