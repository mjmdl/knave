.PHONY: all

CC        ?= gcc
CFLAGS    ?= --std=c17 -Wpedantic -Wall -Wextra
LFLAGS    ?=
BUILD_DIR ?= ./build

SOURCE_DIR := src/kcore
OBJECT_DIR := $(BUILD_DIR)/kcore_obj

SOURCE := $(wildcard $(SOURCE_DIR)/*.c)
OBJECT := $(patsubst $(SOURCE_DIR)/%.c, $(OBJECT_DIR)/%.o, $(SOURCE))
TARGET := $(BUILD_DIR)/libkcore.a

all: $(TARGET)

$(TARGET): $(OBJECT)
	mkdir -p $(BUILD_DIR)
	ar rcs $@ $^

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(OBJECT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
