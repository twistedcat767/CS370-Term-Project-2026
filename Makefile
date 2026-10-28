CC      ?= cc
AR      ?= ar
ARFLAGS := rcs

CFLAGS   := -std=c17 -Wall -Wextra -Werror -Wvla
DEPFLAGS := -MMD -MP
CPPFLAGS := -Isrc/common

SANFLAGS ?=
CFLAGS   += $(SANFLAGS)
LDFLAGS  += $(SANFLAGS)

ASAN_FLAGS := -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer -g

VALGRIND := valgrind --leak-check=full --track-fds=yes --trace-children=yes --error-exitcode=1

BUILD ?= build
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

TEST_SRCS := $(wildcard tests/*.c)
TEST_BINS := $(patsubst tests/%.c,$(BIN)/tests/%,$(TEST_SRCS))
TEST_OBJS := $(patsubst tests/%.c,$(OBJ)/tests/%.o,$(TEST_SRCS))

.SECONDARY: $(TEST_OBJS)

.PHONY: all sensor hub supervisor test asan memcheck clean

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

$(OBJ)/tests/%.o: tests/%.c
	@mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

$(BIN)/tests/%: $(OBJ)/tests/%.o $(LIBCOMMON)
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $< -L$(LIB) -lcommon $(LDLIBS)

test: $(TEST_BINS)
	@fail=0; \
	for t in $(TEST_BINS); do \
		if ./$$t; then echo "PASS $$t"; \
		else echo "FAIL $$t"; fail=1; fi; \
	done; \
	exit $$fail

asan:
	rm -rf build-asan
	$(MAKE) BUILD=build-asan SANFLAGS="$(ASAN_FLAGS)" all test

memcheck: $(TEST_BINS)
	@fail=0; \
	for t in $(TEST_BINS); do \
		if $(VALGRIND) ./$$t; then echo "PASS $$t"; \
		else echo "FAIL $$t"; fail=1; fi; \
	done; \
	exit $$fail

clean:
	rm -rf $(BUILD) build-asan

-include $(shell find $(OBJ) -name '*.d' 2>/dev/null)
