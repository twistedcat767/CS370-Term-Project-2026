CC      ?= cc
AR      ?= ar
ARFLAGS := rcs

CFLAGS   := -std=c17 -Wall -Wextra -Werror -Wvla
DEPFLAGS := -MMD -MP
CPPFLAGS := -Isrc/common

BUILD := build
OBJ   := $(BUILD)/obj
LIB   := $(BUILD)/lib
BIN   := $(BUILD)/bin

COMMON_SRCS     := $(wildcard src/common/*.c)
SENSOR_SRCS     := src/sensors/sensor.c
HUB_SRCS        := src/hub/hub.c
SUPERVISOR_SRCS := src/supervisor/supervisor.c

objs = $(patsubst src/%.c,$(OBJ)/%.o,$(1))

COMMON_OBJS     := $(call objs,$(COMMON_SRCS))
SENSOR_OBJS     := $(call objs,$(SENSOR_SRCS))
HUB_OBJS        := $(call objs,$(HUB_SRCS))
SUPERVISOR_OBJS := $(call objs,$(SUPERVISOR_SRCS))

LIBCOMMON := $(LIB)/libcommon.a
PROGS     := $(BIN)/sensor $(BIN)/hub $(BIN)/supervisor

.PHONY: all sensor hub supervisor clean

all: $(PROGS)

sensor: $(BIN)/sensor
hub: $(BIN)/hub
supervisor: $(BIN)/supervisor

$(OBJ)/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(LIBCOMMON): $(COMMON_OBJS)
	@mkdir -p $(@D)
	$(AR) $(ARFLAGS) $@ $^

$(BIN)/sensor: $(SENSOR_OBJS) $(LIBCOMMON)
$(BIN)/hub: $(HUB_OBJS) $(LIBCOMMON)
$(BIN)/supervisor: $(SUPERVISOR_OBJS) $(LIBCOMMON)

$(PROGS):
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $(filter %.o,$^) -L$(LIB) -lcommon $(LDLIBS)

clean:
	rm -rf $(BUILD)

-include $(shell find $(OBJ) -name '*.d' 2>/dev/null)
