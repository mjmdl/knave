.PHONY: all

CC        ?= gcc
CFLAGS    ?= --std=c17 -Wpedantic -Wall -Wextra
LFLAGS    ?=
BUILD_DIR ?= ./build

CFLAGS += -I../kcore/src/
LFLAGS += -L$(BUILD_DIR) -lkcore

SOURCE_DIR := src/ktest
OBJECT_DIR := $(BUILD_DIR)/ktest_obj

SOURCE := $(wildcard $(SOURCE_DIR)/*.c)
OBJECT := $(patsubst $(SOURCE_DIR)/%.c, $(OBJECT_DIR)/%.o, $(SOURCE))
TARGET := $(BUILD_DIR)/ktest

all: $(TARGET)

$(TARGET): $(OBJECT) $(BUILD_DIR)/libkcore.a
	mkdir -p $(BUILD_DIR)
	$(CC) $(LFLAGS) -o $@ $^

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(OBJECT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
