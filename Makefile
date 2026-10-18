
.DEFAULT_GOAL := all

TARGET ?=

APP_NAME_PREF := hcae
APP_NAME := $(APP_NAME_PREF)$(TARGET)

APP_VER := $(shell cat .version)
APP_VER_MAJOR := $(firstword $(subst ., ,$(APP_VER)))

INST_DIR := $(if $(strip $(PREFIX)),$(PREFIX),/usr/local)
INST_BIN_DIR := $(INST_DIR)/bin
INST_LIB_DIR := $(INST_DIR)/lib
INST_INCLUDE_DIR := $(INST_DIR)/include

SRC_DIR := src
CORE_DIR := core

HEADERS_DIR_NAME := $(APP_NAME_PREF)
HEADERS_DIR := $(SRC_DIR)/$(HEADERS_DIR_NAME)

HEADERS_VERSION_DIR := $(INST_INCLUDE_DIR)/$(HEADERS_DIR_NAME)/$(APP_VER)
HEADERS_TARGET_DIR := $(HEADERS_VERSION_DIR)/$(TARGET)

EMU_FILE := $(APP_NAME)
CONF_FILE := $(APP_NAME)-config

EMU_VERSIONED_FILE := $(EMU_FILE)-$(APP_VER)
CONF_VERSIONED_FILE := $(CONF_FILE)-$(APP_VER)

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
export EMU_VERSIONED_FILE CONF_VERSIONED_FILE
export LIBEMU_SO_VERSIONED_FILE LIBEMU_A_VERSIONED_FILE

.PHONY: all help list-targets check-target __check-target \
        version build install uninstall uninstall-all clean

all:
	@echo 'Please specify a target, e.g. "make install TARGET=32v1"'
	@echo 'Run "make list-targets" to see available targets.'
	@echo 'Run "make help" for more information.'

help:
	@echo 'Available targets:'
	@cat .avail_targets
	@echo
	@echo 'Installation paths:'
	@echo '  Prefix:   $(INST_DIR)'
	@echo '  Binaries: $(INST_BIN_DIR)'
	@echo '  Libraries: $(INST_LIB_DIR)'
	@echo '  Headers:  $(INST_INCLUDE_DIR)/$(HEADERS_DIR_NAME)'
	@echo
	@echo 'Targets:'
	@echo '  help          Print this help text'
	@echo '  list-targets  List available targets'
	@echo '  check-target  Validate TARGET'
	@echo '  build         Build TARGET'
	@echo '  install       Install TARGET'
	@echo '  uninstall     Uninstall TARGET for this version'
	@echo '  uninstall-all Remove all HCAE files under the prefix'
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
	@if [ -z "$(strip $(APP_VER))" ]; then \
		echo 'Error: .version is empty or missing.' >&2; \
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
		"$(HEADERS_VERSION_DIR)" \
		"$(HEADERS_TARGET_DIR)"

	$(INSTALL) -m 755 \
		"$(SRC_DIR)/$(EMU_VERSIONED_FILE)" \
		"$(INST_BIN_DIR)/$(EMU_VERSIONED_FILE)"

	$(INSTALL) -m 755 \
		"$(SRC_DIR)/$(CONF_VERSIONED_FILE)" \
		"$(INST_BIN_DIR)/$(CONF_VERSIONED_FILE)"

	$(INSTALL) -m 644 \
		"$(SRC_DIR)/$(LIBEMU_SO_VERSIONED_FILE)" \
		"$(SRC_DIR)/$(LIBEMU_A_VERSIONED_FILE)" \
		"$(INST_LIB_DIR)/"

	@set -eu; \
	for file in "$(HEADERS_DIR)"/*; do \
		[ -f "$$file" ] || continue; \
		$(INSTALL) -m 644 "$$file" "$(HEADERS_VERSION_DIR)/"; \
	done

	@set -eu; \
	for file in "$(TARGET_TARGET_DIR)/$(TARGET_TARGET_API_DIR)"/*; do \
		[ -f "$$file" ] || continue; \
		$(INSTALL) -m 644 "$$file" "$(HEADERS_TARGET_DIR)/"; \
	done

	@set -eu; \
	for pair in \
		"$(EMU_FILE)|$(EMU_VERSIONED_FILE)" \
		"$(CONF_FILE)|$(CONF_VERSIONED_FILE)"; do \
		name=$${pair%%|*}; \
		versioned=$${pair#*|}; \
		link="$(INST_BIN_DIR)/$$name"; \
		if [ -d "$$link" ] && [ ! -L "$$link" ]; then \
			echo "Error: refusing to replace directory: $$link" >&2; \
			exit 1; \
		fi; \
		$(LN) -sfn "$$versioned" "$$link"; \
	done; \
	for pair in \
		"$(LIBEMU_SO_FILE)|$(LIBEMU_SO_VERSIONED_FILE)|$(LIBEMU_SO_VERSIONED_MAJOR_FILE)" \
		"$(LIBEMU_A_FILE)|$(LIBEMU_A_VERSIONED_FILE)|$(LIBEMU_A_VERSIONED_MAJOR_FILE)"; do \
		unversioned=$${pair%%|*}; \
		rest=$${pair#*|}; \
		versioned=$${rest%%|*}; \
		major=$${rest#*|}; \
		major_link="$(INST_LIB_DIR)/$$major"; \
		unversioned_link="$(INST_LIB_DIR)/$$unversioned"; \
		if [ -d "$$major_link" ] && [ ! -L "$$major_link" ]; then \
			echo "Error: refusing to replace directory: $$major_link" >&2; \
			exit 1; \
		fi; \
		if [ -d "$$unversioned_link" ] && [ ! -L "$$unversioned_link" ]; then \
			echo "Error: refusing to replace directory: $$unversioned_link" >&2; \
			exit 1; \
		fi; \
		$(LN) -sfn "$$versioned" "$$major_link"; \
		$(LN) -sfn "$$major" "$$unversioned_link"; \
	done

uninstall: __check-target
	@set -eu; \
	for pair in \
		"$(EMU_FILE)|$(EMU_VERSIONED_FILE)" \
		"$(CONF_FILE)|$(CONF_VERSIONED_FILE)"; do \
		name=$${pair%%|*}; \
		versioned=$${pair#*|}; \
		link="$(INST_BIN_DIR)/$$name"; \
		if [ -L "$$link" ] && \
		   [ "$$(readlink "$$link")" = "$$versioned" ]; then \
			$(RM) -f "$$link"; \
		fi; \
		$(RM) -f "$(INST_BIN_DIR)/$$versioned"; \
	done

	@set -eu; \
	for pair in \
		"$(LIBEMU_SO_FILE)|$(LIBEMU_SO_VERSIONED_FILE)|$(LIBEMU_SO_VERSIONED_MAJOR_FILE)" \
		"$(LIBEMU_A_FILE)|$(LIBEMU_A_VERSIONED_FILE)|$(LIBEMU_A_VERSIONED_MAJOR_FILE)"; do \
		unversioned=$${pair%%|*}; \
		rest=$${pair#*|}; \
		versioned=$${rest%%|*}; \
		major=$${rest#*|}; \
		unversioned_link="$(INST_LIB_DIR)/$$unversioned"; \
		major_link="$(INST_LIB_DIR)/$$major"; \
		if [ -L "$$unversioned_link" ] && \
		   [ "$$(readlink "$$unversioned_link")" = "$$major" ] && \
		   [ -L "$$major_link" ] && \
		   [ "$$(readlink "$$major_link")" = "$$versioned" ]; then \
			$(RM) -f "$$unversioned_link"; \
		fi; \
		if [ -L "$$major_link" ] && \
		   [ "$$(readlink "$$major_link")" = "$$versioned" ]; then \
			$(RM) -f "$$major_link"; \
		fi; \
		$(RM) -f "$(INST_LIB_DIR)/$$versioned"; \
	done

	$(RM) -rf "$(HEADERS_TARGET_DIR)"

uninstall-all:
	@set -eu; \
	for dir in "$(INST_BIN_DIR)" "$(INST_LIB_DIR)"; do \
		[ -d "$$dir" ] || continue; \
		case "$$dir" in \
			"$(INST_BIN_DIR)") pattern='hcae*' ;; \
			"$(INST_LIB_DIR)") pattern='libhcae*' ;; \
		esac; \
		for file in "$$dir"/$$pattern; do \
			[ -e "$$file" ] || [ -L "$$file" ] || continue; \
			[ -f "$$file" ] || [ -L "$$file" ] || continue; \
			$(RM) -f "$$file"; \
		done; \
	done; \
	$(RM) -rf "$(INST_INCLUDE_DIR)/$(HEADERS_DIR_NAME)"

clean:
	$(MAKE) -C "$(SRC_DIR)" clean
