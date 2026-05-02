# File: Makefile
# Author: Phat Nguyen
# Date: 2026-04-29
# Description: Builds BootManager, Programmer, and App images without IDE dependency.

# -----------------------------------------------------------------------------
# Beginner reading guide
# -----------------------------------------------------------------------------
# Read this file from top to bottom in this order:
#
# 1. User configuration
#    TARGET, OUT_ROOT, toolchain path, vendor paths
#
# 2. Target package
#    make/target-*.mk supplies:
#    - CPU flags
#    - include paths
#    - startup/system files
#    - linker scripts
#    - flash addresses
#
# 3. Source groups
#    Shared source bundles are defined first, then each firmware image becomes
#    just:
#    - image-specific files
#    - shared runtime files
#    - target startup files
#
# 4. Public commands
#    make app
#    make programmer
#    make bootmanager
#    make all
#    make flash-*
#
# 5. Internal build pipeline
#    public target
#    -> image trampoline
#    -> output folder selection
#    -> compile objects
#    -> link ELF
#    -> create bin/hex/lst
# -----------------------------------------------------------------------------

PROJECT_ROOT := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
MAKE_DIR := $(PROJECT_ROOT)/make

#
# User-facing build knobs
#
# TARGET selects which MCU/board memory map and STM32 driver package to use.
# OUT_ROOT is where all generated files are placed.
#
# The vendor package paths below point inside this repo by default.
# A beginner can run the vendor targets once, then build without
# learning machine-specific STM32Cube repository paths.
#
# Advanced users can still create make/config.mk to override these paths
# without editing this Makefile.
#
TARGET ?= stm32f411ce
OUT_ROOT ?= $(PROJECT_ROOT)/out
TOOLCHAIN_PREFIX ?= arm-none-eabi-
CMSIS_CORE_DIR ?= $(PROJECT_ROOT)/vendor/CMSIS_5
CMSIS_DEVICE_F1_DIR ?= $(PROJECT_ROOT)/vendor/cmsis-device-f1
CMSIS_DEVICE_F4_DIR ?= $(PROJECT_ROOT)/vendor/cmsis-device-f4
STM32F1_HAL_DIR ?= $(PROJECT_ROOT)/vendor/stm32f1xx-hal-driver
STM32F4_HAL_DIR ?= $(PROJECT_ROOT)/vendor/stm32f4xx-hal-driver
CUBE_PROGRAMMER_CLI ?= /home/phat/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI
CUBE_PROGRAMMER_CONNECT ?= port=SWD mode=UR reset=HWrst

#
# Local override file
#
# make/config.mk is optional and git-ignored. It is only needed when a
# different machine installs STM32Cube, CubeProgrammer, or the toolchain
# somewhere else.
#
# The leading '-' means "do not fail if the file does not exist".
#
-include $(MAKE_DIR)/config.mk

#
# Toolchain commands
#
# GCC builds C files, G++ links the final ELF so C++ runtime hooks can
# be resolved if the project later adds C++ code.
#
CC := $(TOOLCHAIN_PREFIX)gcc
CXX := $(TOOLCHAIN_PREFIX)g++
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
OBJDUMP := $(TOOLCHAIN_PREFIX)objdump
SIZE := $(TOOLCHAIN_PREFIX)size

#
# Include folders shared by all firmware images
#
# Target-specific include folders are appended later by the selected
# make/target-*.mk file.
#
COMMON_DIRS := \
	$(PROJECT_ROOT) \
	$(PROJECT_ROOT)/bootloader/common \
	$(PROJECT_ROOT)/mcu/stm32 \
	$(PROJECT_ROOT)/mcu/stm32f1 \
	$(PROJECT_ROOT)/mcu/stm32f4 \
	$(PROJECT_ROOT)/port \
	$(PROJECT_ROOT)/startup \
	$(PROJECT_ROOT)/application

#
# Target selection
#
# Each supported target defines:
# - CPU flags
# - preprocessor defines expected by STM32 CMSIS device headers
# - target-specific include paths
# - startup/system files
# - one linker script per image
# - flash addresses for programming
# - sector ranges for erase commands
#
ifeq ($(TARGET),stm32f103)
include $(MAKE_DIR)/target-stm32f103.mk
else ifeq ($(TARGET),stm32f411ce)
include $(MAKE_DIR)/target-stm32f411ce.mk
else
$(error Unsupported TARGET '$(TARGET)')
endif

#
# Common compiler and linker flags
#
# COMMON_CPPFLAGS:
# - CPU flags from the selected target
# - warnings
# - dependency generation
# - include folders
#
# COMMON_CFLAGS / COMMON_CXXFLAGS:
# - language standard
# - C++ is kept "small firmware friendly"
#
# COMMON_LDFLAGS:
# - garbage collect unused sections
# - use small newlib and no host OS syscall layer
#
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

#
# Shared source groups
#
# These groups exist only to make the image lists easier to read.
#
BOOT_COMMON_SRCS := \
	$(PROJECT_ROOT)/bootloader/common/boot_shared.c

RUNTIME_PORT_SRCS := \
	$(PROJECT_ROOT)/port/port_system.c \
	$(PROJECT_ROOT)/port/port_uart.c

STM32_BACKEND_SRCS := \
	$(PROJECT_ROOT)/mcu/stm32/port_uart_stm32.c \
	$(PROJECT_ROOT)/mcu/stm32/port_system_stm32.c

#
# Firmware image source lists
#
# The project builds three separate images:
# - BootManager: reset entry, decides where to go next
# - Programmer: receives update data and writes App flash
# - App: normal firmware that can request Programmer mode
#
# Each image has:
# - its own source list
# - its own linker script
# - its own output folder
# - its own vector-table base address
#
BOOTMANAGER_SRCS := \
	$(PROJECT_ROOT)/bootloader/bootmanager/main.c \
	$(PROJECT_ROOT)/bootloader/common/boot_jump.c \
	$(BOOT_COMMON_SRCS) \
	$(STM32_BACKEND_SRCS) \
	$(RUNTIME_PORT_SRCS) \
	$(STARTUP_SRCS)

PROGRAMMER_SRCS := \
	$(PROJECT_ROOT)/bootloader/programmer/main.c \
	$(PROJECT_ROOT)/bootloader/common/boot_jump.c \
	$(PROJECT_ROOT)/bootloader/common/boot_proto.c \
	$(BOOT_COMMON_SRCS) \
	$(STM32_BACKEND_SRCS) \
	$(PORT_FLASH_BACKEND) \
	$(PROJECT_ROOT)/port/port_flash.c \
	$(RUNTIME_PORT_SRCS) \
	$(STARTUP_SRCS)

APP_SRCS := \
	$(PROJECT_ROOT)/application/app_main.c \
	$(PROJECT_ROOT)/application/app_update.c \
	$(BOOT_COMMON_SRCS) \
	$(STM32_BACKEND_SRCS) \
	$(RUNTIME_PORT_SRCS) \
	$(STARTUP_SRCS)

.PHONY: all help vendor vendor-core vendor-f1 vendor-f4 \
	bootmanager programmer app clean print-config check-config check-toolchain \
	bootmanager-image programmer-image app-image \
	flash-bootmanager flash-programmer flash-app flash-all \
	erase-all erase-bootmanager erase-programmer erase-app check-cube-programmer

#
# High-level entry points
#
# These are the commands humans normally type.
# Example:
#   make TARGET=stm32f411ce all
#   make TARGET=stm32f411ce flash-all
#   make TARGET=stm32f411ce erase-app
#
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
	@echo "  make vendor-core     Clone/update ARM CMSIS core under vendor/"
	@echo "  make vendor-f1       Clone/update STM32F1 driver package + CMSIS device under vendor/"
	@echo "  make vendor-f4       Clone/update STM32F4 driver package + CMSIS device under vendor/"
	@echo "  make vendor          Clone/update all vendor components"
	@echo "  make print-config  Print current build configuration"
	@echo "  make flash-bootmanager  Build and flash BootManager via STM32CubeProgrammer"
	@echo "  make flash-programmer   Build and flash Programmer via STM32CubeProgrammer"
	@echo "  make flash-app          Build and flash App via STM32CubeProgrammer"
	@echo "  make flash-all          Build and flash all images via STM32CubeProgrammer"
	@echo "  make erase-all          Mass erase the target MCU flash"
	@echo "  make erase-bootmanager  Erase the BootManager region"
	@echo "  make erase-programmer   Erase the Programmer region"
	@echo "  make erase-app          Erase the App region"
	@echo ""
	@echo "Configuration:"
	@echo "  1. Run make vendor or make vendor-f4 to clone the needed vendor components"
	@echo "  2. Install GNU Arm Embedded Toolchain"
	@echo "  3. Install STM32CubeProgrammer so STM32_Programmer_CLI is available"
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
	@echo "CMSIS_CORE_DIR=$(CMSIS_CORE_DIR)"
	@echo "CMSIS_DEVICE_F1_DIR=$(CMSIS_DEVICE_F1_DIR)"
	@echo "CMSIS_DEVICE_F4_DIR=$(CMSIS_DEVICE_F4_DIR)"
	@echo "STM32F1_HAL_DIR=$(STM32F1_HAL_DIR)"
	@echo "STM32F4_HAL_DIR=$(STM32F4_HAL_DIR)"
	@echo "TOOLCHAIN_PREFIX=$(TOOLCHAIN_PREFIX)"
	@echo "CUBE_PROGRAMMER_CLI=$(CUBE_PROGRAMMER_CLI)"
	@echo "CUBE_PROGRAMMER_CONNECT=$(CUBE_PROGRAMMER_CONNECT)"
	@echo "OUT_ROOT=$(OUT_ROOT)"

#
# Vendor package fetch targets
#
# These clone the official packages into vendor/.
# They are not committed because they are large third-party dependencies.
# After one successful clone, every machine sees the same folder layout.
#
vendor: vendor-core vendor-f1 vendor-f4

vendor-core:
	@if [ -d "$(CMSIS_CORE_DIR)/.git" ]; then \
		git -C "$(CMSIS_CORE_DIR)" pull --ff-only; \
	elif [ -d "$(CMSIS_CORE_DIR)" ]; then \
		if [ -f "$(CMSIS_CORE_DIR)/CMSIS/Core/Include/core_cm4.h" ]; then \
			echo "Using existing CMSIS core at $(CMSIS_CORE_DIR)"; \
		else \
			echo "$(CMSIS_CORE_DIR) exists but is not a valid CMSIS_5 package."; \
			echo "Remove it or set CMSIS_CORE_DIR to another path."; \
			exit 1; \
		fi; \
	else \
		git clone --depth 1 https://github.com/ARM-software/CMSIS_5.git "$(CMSIS_CORE_DIR)"; \
	fi

vendor-f1:
	@if [ -d "$(CMSIS_DEVICE_F1_DIR)/.git" ]; then \
		git -C "$(CMSIS_DEVICE_F1_DIR)" pull --ff-only; \
	elif [ -d "$(CMSIS_DEVICE_F1_DIR)" ]; then \
		if [ -f "$(CMSIS_DEVICE_F1_DIR)/Include/stm32f103xb.h" ]; then \
			echo "Using existing STM32F1 CMSIS device package at $(CMSIS_DEVICE_F1_DIR)"; \
		else \
			echo "$(CMSIS_DEVICE_F1_DIR) exists but is not a valid cmsis-device-f1 package."; \
			echo "Remove it or set CMSIS_DEVICE_F1_DIR to another path."; \
			exit 1; \
		fi; \
	else \
		git clone --depth 1 https://github.com/STMicroelectronics/cmsis-device-f1.git "$(CMSIS_DEVICE_F1_DIR)"; \
	fi
	@if [ -d "$(STM32F1_HAL_DIR)/.git" ]; then \
		git -C "$(STM32F1_HAL_DIR)" pull --ff-only; \
	elif [ -d "$(STM32F1_HAL_DIR)" ]; then \
		if [ -f "$(STM32F1_HAL_DIR)/Inc/stm32f1xx_hal.h" ] && [ -f "$(STM32F1_HAL_DIR)/Src/stm32f1xx_hal.c" ]; then \
			echo "Using existing STM32F1 HAL package at $(STM32F1_HAL_DIR)"; \
		else \
			echo "$(STM32F1_HAL_DIR) exists but is not a valid stm32f1xx-hal-driver package."; \
			echo "Remove it or set STM32F1_HAL_DIR to another path."; \
			exit 1; \
		fi; \
	else \
		git clone --depth 1 https://github.com/STMicroelectronics/stm32f1xx-hal-driver.git "$(STM32F1_HAL_DIR)"; \
	fi

vendor-f4:
	@if [ -d "$(CMSIS_DEVICE_F4_DIR)/.git" ]; then \
		git -C "$(CMSIS_DEVICE_F4_DIR)" pull --ff-only; \
	elif [ -d "$(CMSIS_DEVICE_F4_DIR)" ]; then \
		if [ -f "$(CMSIS_DEVICE_F4_DIR)/Include/stm32f411xe.h" ]; then \
			echo "Using existing STM32F4 CMSIS device package at $(CMSIS_DEVICE_F4_DIR)"; \
		else \
			echo "$(CMSIS_DEVICE_F4_DIR) exists but is not a valid cmsis-device-f4 package."; \
			echo "Remove it or set CMSIS_DEVICE_F4_DIR to another path."; \
			exit 1; \
		fi; \
	else \
		git clone --depth 1 https://github.com/STMicroelectronics/cmsis-device-f4.git "$(CMSIS_DEVICE_F4_DIR)"; \
	fi
	@if [ -d "$(STM32F4_HAL_DIR)/.git" ]; then \
		git -C "$(STM32F4_HAL_DIR)" pull --ff-only; \
	elif [ -d "$(STM32F4_HAL_DIR)" ]; then \
		if [ -f "$(STM32F4_HAL_DIR)/Inc/stm32f4xx_hal.h" ] && [ -f "$(STM32F4_HAL_DIR)/Src/stm32f4xx_hal.c" ]; then \
			echo "Using existing STM32F4 HAL package at $(STM32F4_HAL_DIR)"; \
		else \
			echo "$(STM32F4_HAL_DIR) exists but is not a valid stm32f4xx-hal-driver package."; \
			echo "Remove it or set STM32F4_HAL_DIR to another path."; \
			exit 1; \
		fi; \
	else \
		git clone --depth 1 https://github.com/STMicroelectronics/stm32f4xx-hal-driver.git "$(STM32F4_HAL_DIR)"; \
	fi

#
# Environment checks
#
# check-config:
# - compiler exists
# - required vendor folders for the selected target exist
#
# check-cube-programmer:
# - only needed for flash/erase targets
# - normal builds should still work without an ST-LINK connected
#
check-toolchain:
	@if ! command -v $(CC) >/dev/null 2>&1; then \
		echo "$(CC) not found in PATH."; \
		echo "Set TOOLCHAIN_PREFIX or add the GNU Arm Embedded Toolchain to PATH."; \
		exit 1; \
	fi

check-config: check-toolchain
	@for dir in $(CHECK_COMPONENT_DIRS); do \
		if [ ! -d "$$dir" ]; then \
			echo "$$dir does not exist."; \
			echo "Run $(CHECK_COMPONENT_HINT) or override the vendor paths in make/config.mk."; \
			exit 1; \
		fi; \
	done

check-cube-programmer:
	@if [ ! -x "$(CUBE_PROGRAMMER_CLI)" ] && ! command -v "$(CUBE_PROGRAMMER_CLI)" >/dev/null 2>&1; then \
		echo "$(CUBE_PROGRAMMER_CLI) not found."; \
		echo "Set CUBE_PROGRAMMER_CLI to STM32_Programmer_CLI or STM32_Programmer_CLI.exe."; \
		exit 1; \
	fi

#
# Per-image public build targets
#
# Each public image target sets a small "image context":
# - IMAGE: image name used in output filenames/folders
# - IMAGE_SRCS: source files that belong to that image
# - IMAGE_LD: linker script that places that image in flash
# - IMAGE_DEFINES: one define that lets startup code know which image it is
#
# After that, all three images share the same internal pipeline.
#
bootmanager: IMAGE := bootmanager
bootmanager: IMAGE_SRCS := $(BOOTMANAGER_SRCS)
bootmanager: IMAGE_LD := $(BOOTMANAGER_LD)
bootmanager: IMAGE_DEFINES := -DIMAGE_BOOTMANAGER
bootmanager: check-config bootmanager-image

programmer: IMAGE := programmer
programmer: IMAGE_SRCS := $(PROGRAMMER_SRCS)
programmer: IMAGE_LD := $(PROGRAMMER_LD)
programmer: IMAGE_DEFINES := -DIMAGE_PROGRAMMER
programmer: check-config programmer-image

app: IMAGE := app
app: IMAGE_SRCS := $(APP_SRCS)
app: IMAGE_LD := $(APP_LD)
app: IMAGE_DEFINES := -DIMAGE_APP
app: check-config app-image

#
# Flash and erase targets
#
# Flash commands write a raw .bin plus its destination address.
# Erase commands use target-specific sectors/pages supplied by the target file.
#
flash-bootmanager: bootmanager check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -d $(OUT_ROOT)/$(TARGET)/bootmanager/bootmanager.bin $(BOOTMANAGER_ADDR) -v -rst

flash-programmer: programmer check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -d $(OUT_ROOT)/$(TARGET)/programmer/programmer.bin $(PROGRAMMER_ADDR) -v -rst

flash-app: app check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -d $(OUT_ROOT)/$(TARGET)/app/app.bin $(APP_ADDR) -v -rst

flash-all: all check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -d $(OUT_ROOT)/$(TARGET)/bootmanager/bootmanager.bin $(BOOTMANAGER_ADDR) -v
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -d $(OUT_ROOT)/$(TARGET)/programmer/programmer.bin $(PROGRAMMER_ADDR) -v
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -d $(OUT_ROOT)/$(TARGET)/app/app.bin $(APP_ADDR) -v -rst

erase-all: check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -e all -rst

erase-bootmanager: check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -e [$(BOOTMANAGER_ERASE_SECTORS)] -rst

erase-programmer: check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -e [$(PROGRAMMER_ERASE_SECTORS)] -rst

erase-app: check-cube-programmer
	$(CUBE_PROGRAMMER_CLI) -c $(CUBE_PROGRAMMER_CONNECT) -e [$(APP_ERASE_SECTORS)] -rst

#
# Generic image build trampoline
#
# Why the extra recursion?
#
# Public target:
#   make app
#
# Stage 1:
#   choose image context (name, sources, linker script, defines)
#
# Stage 2:
#   compute output folder: out/<target>/<image>/
#
# Stage 3:
#   compile -> link -> create bin/hex/lst
#
# This keeps one shared build pipeline instead of copy/pasting nearly the
# same rules three times.
#
bootmanager-image programmer-image app-image:
	@$(MAKE) --no-print-directory \
		IMAGE=$(IMAGE) \
		IMAGE_SRCS='$(IMAGE_SRCS)' \
		IMAGE_LD='$(IMAGE_LD)' \
		IMAGE_DEFINES='$(IMAGE_DEFINES)' \
		image-internal

image-internal:
	@mkdir -p $(OUT_ROOT)/$(TARGET)/$(IMAGE)
	@$(MAKE) --no-print-directory \
		IMAGE=$(IMAGE) \
		IMAGE_SRCS='$(IMAGE_SRCS)' \
		IMAGE_LD='$(IMAGE_LD)' \
		IMAGE_DEFINES='$(IMAGE_DEFINES)' \
		IMAGE_OUT=$(OUT_ROOT)/$(TARGET)/$(IMAGE) \
		image-link

image-link: $(IMAGE_OUT)/$(IMAGE).elf $(IMAGE_OUT)/$(IMAGE).bin $(IMAGE_OUT)/$(IMAGE).hex $(IMAGE_OUT)/$(IMAGE).lst
	@$(SIZE) $(IMAGE_OUT)/$(IMAGE).elf

#
# Object file mapping
#
# Project files keep their folder structure under:
#   out/<target>/<image>/obj/...
#
# Vendor files that live outside PROJECT_ROOT are placed under:
#   out/<target>/<image>/obj/vendor/
#
# This keeps generated object paths predictable and easy to browse.
#
IMAGE_OBJS = \
	$(patsubst $(PROJECT_ROOT)/%.c,$(IMAGE_OUT)/obj/%.o,$(filter $(PROJECT_ROOT)/%.c,$(filter %.c,$(IMAGE_SRCS)))) \
	$(patsubst $(PROJECT_ROOT)/%.cpp,$(IMAGE_OUT)/obj/%.o,$(filter $(PROJECT_ROOT)/%.cpp,$(filter %.cpp,$(IMAGE_SRCS)))) \
	$(foreach src,$(filter-out $(PROJECT_ROOT)/%,$(filter %.c,$(IMAGE_SRCS))),$(IMAGE_OUT)/obj/vendor/$(notdir $(src:.c=.o))) \
	$(foreach src,$(filter-out $(PROJECT_ROOT)/%,$(filter %.cpp,$(IMAGE_SRCS))),$(IMAGE_OUT)/obj/vendor/$(notdir $(src:.cpp=.o)))
IMAGE_CFLAGS = $(COMMON_CFLAGS) $(IMAGE_DEFINES)
IMAGE_CXXFLAGS = $(COMMON_CXXFLAGS) $(IMAGE_DEFINES)
IMAGE_DEPS = $(IMAGE_OBJS:.o=.d)
vpath %.c $(sort $(dir $(filter-out $(PROJECT_ROOT)/%,$(filter %.c,$(IMAGE_SRCS)))))
vpath %.cpp $(sort $(dir $(filter-out $(PROJECT_ROOT)/%,$(filter %.cpp,$(IMAGE_SRCS)))))

#
# Link and artifact generation
#
# Final artifacts per image:
# - .elf : linked firmware
# - .bin : raw binary for direct flashing
# - .hex : Intel HEX
# - .lst : section + disassembly listing
# - .map : linker map
#
$(IMAGE_OUT)/$(IMAGE).elf: $(IMAGE_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON_LDFLAGS) -T$(IMAGE_LD) -Wl,-Map,$(IMAGE_OUT)/$(IMAGE).map -o $@ $^

$(IMAGE_OUT)/$(IMAGE).bin: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJCOPY) -O binary $< $@

$(IMAGE_OUT)/$(IMAGE).hex: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJCOPY) -O ihex $< $@

$(IMAGE_OUT)/$(IMAGE).lst: $(IMAGE_OUT)/$(IMAGE).elf
	$(OBJDUMP) -h -S $< > $@

#
# Compile rules
#
# IMAGE_CFLAGS / IMAGE_CXXFLAGS = common flags + one image-specific define.
# That image-specific define is mainly used by startup code to select the
# correct vector-table base.
#
$(IMAGE_OUT)/obj/%.o: $(PROJECT_ROOT)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(IMAGE_CFLAGS) -c $< -o $@

$(IMAGE_OUT)/obj/%.o: $(PROJECT_ROOT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(IMAGE_CXXFLAGS) -c $< -o $@

$(IMAGE_OUT)/obj/vendor/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(IMAGE_CFLAGS) -c $< -o $@

$(IMAGE_OUT)/obj/vendor/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(IMAGE_CXXFLAGS) -c $< -o $@

#
# Include generated dependency files
#
# Safe on the first build because missing .d files are ignored.
#
-include $(IMAGE_DEPS)

#
# Cleanup
#
# All generated files live under OUT_ROOT, so clean can remove that folder
# without touching source code.
#
clean:
	rm -rf $(OUT_ROOT)
