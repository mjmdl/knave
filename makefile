.PHONY: all clean kcore ktest

CC        := gcc
AR        := ar
CFLAGS    := --std=c17 -Wpedantic -Wall -Wextra
LFLAGS    :=
BUILD_DIR := ./build

export CC AR CFLAGS LFLAGS BUILD_DIR

all: ktest

kcore:
	$(MAKE) -C kcore BUILD_DIR=../$(BUILD_DIR)

ktest: kcore
	$(MAKE) -C ktest BUILD_DIR=../$(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
