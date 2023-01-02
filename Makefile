TARGET := mhnac

CC := cc
PKG_CONFIG = pkg-config

PACKAGES := libnfc libfreefare

CFLAGS := -Wall -Wextra -std=c11 -pedantic

CFLAGS += -Wdeprecated-declarations
CFLAGS += `$(PKG_CONFIG) --cflags $(PACKAGES)`
LIBS := `$(PKG_CONFIG) --libs $(PACKAGES)` -lm

RELEASE := -Werror
OPTIMIZATION := -O2
DEBUGGING := -O0 -g -DDEBUG

BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)/obj
SRC_DIR := ./src

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

### Linux

.PHONY: default
default: COMPILE_OPTS := $(OPTIMIZATION)

default: $(BUILD_DIR)/$(TARGET)

.PHONY: debug
debug: COMPILE_OPTS := $(DEBUGGING)

debug: $(BUILD_DIR)/$(TARGET)

.PHONY: release
release: COMPILE_OPTS := `$(RELEASE) $(OPTIMIZATION)`

release: $(BUILD_DIR)/$(TARGET)

###

# Binary
$(BUILD_DIR)/$(TARGET): $(OBJ_DIR) $(OBJS)
	@set -xe
	$(CC) $(OBJS) -o $@ $(LIBS)

# Object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@set -xe
	$(CC) $(CFLAGS) $(COMPILE_OPTS) -c $< -o $@

### Utils

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

.PHONY: clean
clean:
	@rm -r $(OBJ_DIR)

.PHONY: clear
clear:
	@rm -r $(BUILD_DIR)

.PHONY: install
install:
	@cp $(BUILD_DIR)/$(TARGET) /usr/bin/

.PHONY: run
run:
	@$(BUILD_DIR)/$(TARGET) $(ARGS)
