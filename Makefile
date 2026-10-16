
.DEFAULT_GOAL := all

TARGET ?=

APP_NAME_PREF := hcae
APP_NAME := $(APP_NAME_PREF)$(TARGET)

APP_VER := $(shell cat .version)
APP_VER_MAJOR := $(firstword $(subst ., ,$(APP_VER)))

USR_DIR := $(if $(strip $(PREFIX)),$(PREFIX),/usr)
INST_DIR := $(if $(strip $(PREFIX)),$(PREFIX),/usr/local)
INST_BIN_DIR := $(INST_DIR)/bin
INST_LIB_DIR := $(INST_DIR)/lib
INST_INCLUDE_DIR := $(INST_DIR)/include

SRC_DIR := src
CORE_DIR := core

HEADERS_DIR_NAME := $(APP_NAME_PREF)
HEADERS_DIR := $(SRC_DIR)/$(HEADERS_DIR_NAME)

EMU_FILE := $(APP_NAME)
CONF_FILE := $(APP_NAME)-config

LIBEMU_SO_FILE := lib$(APP_NAME).so
LIBEMU_A_FILE := lib$(APP_NAME).a

LIBEMU_SO_VERSIONED_FILE := $(LIBEMU_SO_FILE).$(APP_VER)
LIBEMU_A_VERSIONED_FILE := $(LIBEMU_A_FILE).$(APP_VER)

LIBEMU_SO_VERSIONED_MAJOR_FILE := $(LIBEMU_SO_FILE).$(APP_VER_MAJOR)
LIBEMU_A_VERSIONED_MAJOR_FILE := $(LIBEMU_A_FILE).$(APP_VER_MAJOR)

TARGET_DIR := $(SRC_DIR)/target
TARGET_TARGET_DIR := $(TARGET_DIR)/$(TARGET)
TARGET_TARGET_API_DIR := api

RM := rm
INSTALL := install
LN := ln

CC := clang
LD := $(CC)
AR := ar

export CC LD AR CORE_DIR
export EMU_FILE CONF_FILE
export LIBEMU_SO_VERSIONED_FILE LIBEMU_A_VERSIONED_FILE

.PHONY: all help list-targets check-target __check-target \
        version build install uninstall clean

all:
	@echo 'Please specify a target, e.g. "make install TARGET=32v1"'
	@echo 'Run "make list-targets" to see available targets.'
	@echo 'Run "make help" for more information.'

help:
	@echo 'Available targets:'
	@cat .avail_targets
	@echo
	@echo 'Install Path: $(INST_DIR)'
	@echo 'Install Path for Binaries: $(INST_BIN_DIR)'
	@echo 'Install Path for Libraries: $(INST_LIB_DIR)'
	@echo 'Install Path for Headers: $(INST_INCLUDE_DIR)'
	@echo
	@echo 'Options:'
	@echo '  help          Print this help text'
	@echo '  list-targets  List available targets'
	@echo '  check-target  Validate TARGET'
	@echo '  install       Build and install TARGET'
	@echo '  uninstall     Uninstall TARGET'
	@echo '  clean         Clean build artifacts'
	@echo '  version       Print the current version'
	@echo
	@echo 'Variables:'
	@echo '  TARGET        Required target identifier'
	@echo '  PREFIX        Optional installation prefix'

list-targets:
	@echo 'Available targets:'
	@cat .avail_targets

check-target: __check-target
	@echo 'Target "$(TARGET)" exists.'

__check-target:
	@if [ -z "$(strip $(TARGET))" ]; then \
		echo 'Error: TARGET was not specified.' >&2; \
		echo 'Example: make install TARGET=32v1' >&2; \
		exit 1; \
	fi
	@if [ ! -d "$(TARGET_TARGET_DIR)" ]; then \
		echo 'Error: Invalid target "$(TARGET)".' >&2; \
		echo 'Run "make list-targets" to see available targets.' >&2; \
		exit 1; \
	fi
	@if [ ! -d "$(TARGET_TARGET_DIR)/$(TARGET_TARGET_API_DIR)" ]; then \
		echo 'Error: API directory missing for target "$(TARGET)".' >&2; \
		exit 1; \
	fi

version:
	@echo '$(APP_VER)'

build: __check-target
	$(MAKE) -C "$(SRC_DIR)" TARGET="$(TARGET)"

install: build
	$(INSTALL) -d -m 755 \
		"$(INST_BIN_DIR)" \
		"$(INST_LIB_DIR)" \
		"$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)"

	$(INSTALL) -m 755 \
		"$(SRC_DIR)/$(EMU_FILE)" \
		"$(INST_BIN_DIR)/$(EMU_FILE)"

	$(INSTALL) -m 644 \
		"$(SRC_DIR)/$(LIBEMU_SO_VERSIONED_FILE)" \
		"$(SRC_DIR)/$(LIBEMU_A_VERSIONED_FILE)" \
		"$(INST_LIB_DIR)"

	$(INSTALL) -d -m 755 \
		"$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)/$(APP_VER)" \
		"$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)/$(APP_VER)/$(TARGET)"

	@set -eu; \
	for file in "$(HEADERS_DIR)"/*; do \
		[ -f "$$file" ] || continue; \
		$(INSTALL) -m 644 "$$file" \
			"$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)/$(APP_VER)/"; \
	done

	@set -eu; \
	for file in "$(TARGET_TARGET_DIR)/$(TARGET_TARGET_API_DIR)"/*; do \
		[ -f "$$file" ] || continue; \
		$(INSTALL) -m 644 "$$file" \
			"$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)/$(APP_VER)/$(TARGET)/"; \
	done

	@set -eu; \
	for link in \
		"$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)/$(APP_VER_MAJOR)" \
		"$(INST_LIB_DIR)/$(LIBEMU_SO_VERSIONED_MAJOR_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_A_VERSIONED_MAJOR_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_SO_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_A_FILE)"; do \
		if [ -d "$$link" ] && [ ! -L "$$link" ]; then \
			echo "Error: refusing to replace real directory: $$link" >&2; \
			exit 1; \
		fi; \
	done; \
	$(LN) -sfn "$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)/$(APP_VER)" \
		"$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)/$(APP_VER_MAJOR)"; \
	$(LN) -sfn "$(INST_LIB_DIR)/$(LIBEMU_SO_VERSIONED_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_SO_VERSIONED_MAJOR_FILE)"; \
	$(LN) -sfn "$(INST_LIB_DIR)/$(LIBEMU_A_VERSIONED_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_A_VERSIONED_MAJOR_FILE)"; \
	$(LN) -sfn "$(INST_LIB_DIR)/$(LIBEMU_SO_VERSIONED_MAJOR_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_SO_FILE)"; \
	$(LN) -sfn "$(INST_LIB_DIR)/$(LIBEMU_A_VERSIONED_MAJOR_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_A_FILE)"

uninstall: __check-target
	$(RM) -f \
		"$(INST_BIN_DIR)/$(EMU_FILE)" \
		"$(INST_BIN_DIR)/$(CONF_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_SO_VERSIONED_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_A_VERSIONED_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_SO_FILE)" \
		"$(INST_LIB_DIR)/$(LIBEMU_A_FILE)"

	@set -eu; \
	include_root="$(INST_INCLUDE_DIR)/$(APP_NAME_PREF)"; \
	version_dir="$$include_root/$(APP_VER)"; \
	target_dir="$$version_dir/$(TARGET)"; \
	include_major_link="$$include_root/$(APP_VER_MAJOR)"; \
	$(RM) -rf "$$target_dir"; \
	if [ -d "$$version_dir" ] && \
	   ! find "$$version_dir" -mindepth 1 -maxdepth 1 -type d \
	       -print -quit | grep -q .; then \
		$(RM) -rf "$$version_dir"; \
	fi; \
	if [ -L "$$include_major_link" ] && \
	   [ "$$(readlink "$$include_major_link")" = "$$version_dir" ]; then \
		$(RM) -f "$$include_major_link"; \
	fi; \
	if [ -d "$$include_root" ] && \
	   [ -z "$$(find "$$include_root" -mindepth 1 -print -quit)" ]; then \
		$(RM) -rf "$$include_root"; \
	fi

	@set -eu; \
	for pair in \
		"$(LIBEMU_SO_VERSIONED_MAJOR_FILE)|$(LIBEMU_SO_VERSIONED_FILE)|$(LIBEMU_SO_FILE)" \
		"$(LIBEMU_A_VERSIONED_MAJOR_FILE)|$(LIBEMU_A_VERSIONED_FILE)|$(LIBEMU_A_FILE)"; do \
		major=$${pair%%|*}; \
		rest=$${pair#*|}; \
		versioned=$${rest%%|*}; \
		unversioned=$${rest#*|}; \
		major_link="$(INST_LIB_DIR)/$$major"; \
		versioned_link="$(INST_LIB_DIR)/$$versioned"; \
		unversioned_link="$(INST_LIB_DIR)/$$unversioned"; \
		if [ -L "$$major_link" ] && \
		   [ "$$(readlink "$$major_link")" = "$(INST_LIB_DIR)/$$versioned" ]; then \
			$(RM) -f "$$major_link"; \
		fi; \
		if [ -L "$$unversioned_link" ] && \
		   [ "$$(readlink "$$unversioned_link")" = "$$major" ]; then \
			if [ -e "$$major_link" ] || [ -L "$$major_link" ]; then \
				:; \
			else \
				$(RM) -f "$$unversioned_link"; \
			fi; \
		fi; \
	done

clean:
	$(MAKE) -C "$(SRC_DIR)" clean TARGET="$(TARGET)"
