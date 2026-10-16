TARGET ?=

TARGET_DIR := target
TARGET_TARGET_DIR := $(TARGET_DIR)/$(TARGET)

EMU_MAIN_O_FILE := $(BUILD_DIR)/core/emu-main.o
CONF_MAIN_O_FILE := $(BUILD_DIR)/core/config-main.o

BUILD_DIR := build/$(TARGET)

CORE_C_FILES := $(shell find core -type f -name '*.c')
TARGET_C_FILES := $(shell if [ -n "$(TARGET)" ] && [ -d "$(TARGET_TARGET_DIR)" ]; then find "$(TARGET_TARGET_DIR)" -type f -name '*.c'; fi)

C_FILES := $(CORE_C_FILES) $(TARGET_C_FILES)
O_FILES := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_FILES))
D_FILES := $(O_FILES:.o=.d)

EMU_MAIN_O_FILE := $(BUILD_DIR)/core/emu-main.o
CONF_MAIN_O_FILE := $(BUILD_DIR)/core/config-main.o

LIB_O_FILES := $(filter-out $(EMU_MAIN_O_FILE) $(CONF_MAIN_O_FILE),$(O_FILES))
EMU_O_FILES := $(filter-out $(CONF_MAIN_O_FILE),$(O_FILES))
CONF_O_FILES := $(filter-out $(EMU_MAIN_O_FILE),$(O_FILES))

CC_FLAGS := \
	-Wall -Wextra \
	-I. -Icore -I$(TARGET_TARGET_DIR) \
	-ffunction-sections -fdata-sections \
	-fPIC -O2

DEP_FLAGS := -MMD -MP
LD_FLAGS := -Wl,--gc-sections -Wl,--strip-all

RM := rm
INSTALL := install

.PHONY: all clean

all: $(EMU_FILE) $(CONF_FILE) \
	$(LIBEMU_SO_VERSIONED_FILE) $(LIBEMU_A_VERSIONED_FILE)

$(EMU_FILE): $(EMU_O_FILES)
	$(LD) $^ -o $@ $(LD_FLAGS)

$(CONF_FILE): $(CONF_O_FILES)
	$(LD) $^ -o $@ $(LD_FLAGS)

$(LIBEMU_SO_VERSIONED_FILE): $(LIB_O_FILES)
	$(LD) -shared $^ -o $@ $(LD_FLAGS)

$(LIBEMU_A_VERSIONED_FILE): $(LIB_O_FILES)
	$(AR) rcs $@ $^

$(BUILD_DIR)/%.o: %.c
	@mkdir -p "$(@D)"
	$(CC) $(CC_FLAGS) $(DEP_FLAGS) \
		-MF "$(@:.o=.d)" -MT "$@" \
		-c "$<" -o "$@"

clean:
	$(RM) -rf build

-include $(D_FILES)
