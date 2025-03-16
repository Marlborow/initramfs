SHELL := /bin/bash

# Program name (the final executable)
PROGRAM   := init
LINUX_DIR := linux-6.13.7

# Tools and Flags
CC        := gcc
CFLAGS    := -Wall -Wextra -std=c11
NASM      := nasm
NASMFLAGS := -f elf64

# Libraries to link (adjust or remove as needed)
LIBS :=

# Directories
SRC_DIR    := src
OBJ_DIR    := obj
BUILD_DIR  := build
RELEASE_DIR:= $(BUILD_DIR)/release
DEBUG_DIR  := $(BUILD_DIR)/debug
BIN_DIR    := $(RELEASE_DIR)/bin

# ANSI color codes for pretty output
RED    := \033[0;31m
GREEN  := \033[0;32m
YELLOW := \033[0;33m
NC     := \033[0m

# Gather all .c and .asm files under src/
C_SRCS   := $(shell find $(SRC_DIR) -type f -name '*.c' ! -path '$(SRC_DIR)/modules/*')
ASM_SRCS := $(shell find $(SRC_DIR) -type f -name '*.asm')

OBJS := \
  $(patsubst $(SRC_DIR)/%.c,   $(OBJ_DIR)/%.o, $(C_SRCS)) \
  $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(ASM_SRCS))

OBJS := $(filter-out $(OBJ_DIR)/modules/%, $(OBJS))



###############################################################################
# Main Program Build Targets
###############################################################################
.PHONY: all
all: release

.PHONY: release
release: $(RELEASE_DIR)/$(PROGRAM)

$(RELEASE_DIR)/$(PROGRAM): $(OBJS)
	@mkdir -p $(RELEASE_DIR)
	@echo -e "$(GREEN)LINKING RELEASE $(PROGRAM)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -static
	@echo -e "init | cpio -H newc -o" > $(RELEASE_DIR)/$(PROGRAM).cpio

.PHONY: debug
debug: CFLAGS += -g -DDEBUG
debug: $(DEBUG_DIR)/$(PROGRAM)

$(DEBUG_DIR)/$(PROGRAM): $(OBJS)
	@mkdir -p $(DEBUG_DIR)
	@echo -e "$(GREEN)LINKING DEBUG $(PROGRAM)...$(NC)"
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

###############################################################################
# Compile C files into obj/ folder
###############################################################################
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)Compiling C: $<$(NC)"
	@$(CC) $(CFLAGS) -c $< -o $@

###############################################################################
# Assemble .asm files into obj/ folder
###############################################################################
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)Assembling ASM: $<$(NC)"
	@$(NASM) $(NASMFLAGS) -o $@ $<

###############################################################################
# Run target: Build and then run the program
###############################################################################
.PHONY: run
run: release
	@echo -e "BINARY $(YELLOW)PROPERTIES$(NC)"
	@echo -e "----------------------------------------------------------"
	@echo -e "$(YELLOW)Binary Name$(NC): $(PROGRAM)"
	@stat --printf="$(YELLOW)Binary Creation Date$(NC): %w\n" $(RELEASE_DIR)/$(PROGRAM) || true
	@echo -e "----------------------------------------------------------"
	@echo -e "$(YELLOW)Binary Size:$(NC)"
	@size $(RELEASE_DIR)/$(PROGRAM)
	@echo -e "----------------------------------------------------------"
	@echo -e "$(YELLOW)Dynamic Linking:$(NC)"
	@ldd $(RELEASE_DIR)/$(PROGRAM) || true
	@echo -e "----------------------------------------------------------"
	@echo -e "$(GREEN)RUNNING $(NC)$(PROGRAM)$(GREEN)...$(NC)"
	@echo -e "----------------------------------------------------------"
	@./$(RELEASE_DIR)/$(PROGRAM)

###############################################################################
# Clean
###############################################################################
.PHONY: clean
clean:
	@echo -e "$(RED)Cleaning build artifacts...$(NC)"
	@rm -rf $(OBJ_DIR) $(BUILD_DIR)

###############################################################################
# Linux Build Targets
###############################################################################
.PHONY: linux-rebuild
linux-rebuild: clean release
	@echo -e "$(RED)Cleaning $(LINUX_DIR)...$(NC)"
	$(MAKE) -C $(HOME)/$(LINUX_DIR) clean

	@echo -e "$(GREEN)Creating initramfs $(PROGRAM).cpio...$(NC)"
	@cd $(RELEASE_DIR) && find . | cpio -H newc -o --owner=root:root > $(PROGRAM).cpio

	@echo -e "$(GREEN)Building Linux ISO with initramfs...$(NC)"
	$(MAKE) -C $(HOME)/$(LINUX_DIR) isoimage FDARGS="initrd=/init.cpio" FDINITRD=$(HOME)/Source/Userspace/$(RELEASE_DIR)/$(PROGRAM).cpio

.PHONY: linux
linux: release
	@echo -e "$(GREEN)Creating initramfs $(PROGRAM).cpio...$(NC)"
	@cd $(RELEASE_DIR) && find . -mindepth 1 | cpio -H newc -o --owner=root:root > $(PROGRAM).cpio

	@echo -e "$(GREEN)Building Linux ISO with initramfs...$(NC)"
	$(MAKE) -C $(HOME)/$(LINUX_DIR) isoimage FDARGS="init=/init" FDINITRD=$(HOME)/Source/Userspace/$(RELEASE_DIR)/$(PROGRAM).cpio


.PHONY: QEMU/rootfs.img
QEMU/rootfs.img:
	@echo -e "$(GREEN)Creating QEMU/rootfs.img (128MB ext4)...$(NC)"
	mkdir -p QEMU
	qemu-img create -f raw QEMU/rootfs.img 128M
	mkfs.ext4 QEMU/rootfs.img


.PHONY: run-linux
run-linux:
	@echo -e "$(GREEN)Running Linux in QEMU with 9P shared folder...$(NC)"
	qemu-system-x86_64 \
		-m 512M \
		-kernel $(HOME)/linux-6.13.7/arch/x86/boot/bzImage \
		-initrd build/release/init.cpio \
		-fsdev local,id=fsdev0,path=/home/sirrus/shared,security_model=mapped \
		-device virtio-9p-pci,fsdev=fsdev0,mount_tag=hostshare \
		-append "root=/dev/ram rw nokaslr" \







###############################################################################
# Module Build Targets
###############################################################################

MODULES_DIR := src/modules
MODULE_C_SRCS := $(shell find $(MODULES_DIR) -type f -name '*.c')

# Generate output binary paths in /build/release/bin/
MODULE_BINARIES := $(patsubst $(MODULES_DIR)/%.c, $(BIN_DIR)/%, $(MODULE_C_SRCS))

# All object files in obj/libs/
LIB_OBJS := $(shell find $(OBJ_DIR)/libs -type f -name '*.o')

.PHONY: modules
modules: check-build $(MODULE_BINARIES)

.PHONY: check-build
check-build:
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		echo -e "$(YELLOW)Build directory missing, running 'make release' first...$(NC)"; \
		$(MAKE) release; \
	fi

# Compile module C files into binaries
$(BIN_DIR)/%: $(MODULES_DIR)/%.c $(LIB_OBJS)
	@mkdir -p $(BIN_DIR)  # Ensure bin/ exists
	@modname=$*; \
	echo -e "$(GREEN)Building module '$${modname}' $(NC)"; \
	$(CC) $(CFLAGS) -static -o $@ $< $(LIB_OBJS) >/dev/null 2>&1

