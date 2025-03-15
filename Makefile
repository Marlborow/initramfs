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
C_SRCS   := $(shell find $(SRC_DIR) -type f -name '*.c')
ASM_SRCS := $(shell find $(SRC_DIR) -type f -name '*.asm')

# Convert them into .o paths under obj/
OBJS := \
  $(patsubst $(SRC_DIR)/%.c,   $(OBJ_DIR)/%.o, $(C_SRCS)) \
  $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(ASM_SRCS))

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
	@cd $(RELEASE_DIR) && find . | cpio -H newc -o --owner=root:root > $(PROGRAM).cpio

	@echo -e "$(GREEN)Building Linux ISO with initramfs...$(NC)"
	$(MAKE) -C $(HOME)/$(LINUX_DIR) isoimage FDARGS="initrd=/init.cpio" FDINITRD=$(HOME)/Source/Userspace/$(RELEASE_DIR)/$(PROGRAM).cpio

.PHONY: run-linux
run-linux:
	@qemu-system-x86_64 -cdrom $(HOME)/$(LINUX_DIR)/arch/x86/boot/image.iso

.PHONY: testcpio
testcpio:
	@mkdir -p $(HOME)/Source/Userspace/$(BUILD_DIR)/testing
	@cd $(HOME)/Source/Userspace/$(BUILD_DIR)/testing && cpio -idv < $(HOME)/Source/Userspace/$(RELEASE_DIR)/$(PROGRAM).cpio > test.txt

###############################################################################
# Module Build Targets
###############################################################################
MODULES_DIR := src/modules
MODULE_C_SRCS := $(shell find $(MODULES_DIR) -type f -name '*.c')
MODULE_BINARIES := $(addprefix $(BIN_DIR)/, $(basename $(notdir $(MODULE_C_SRCS))))

.PHONY: modules
modules: $(MODULE_BINARIES)

$(BIN_DIR)/%: $(MODULES_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo -e "$(GREEN)Building module '$*'...$(NC)"
	@$(CC) $(CFLAGS) -static -o $@ $<

