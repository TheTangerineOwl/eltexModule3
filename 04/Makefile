CC = gcc
CFLAGS = -Wall -Wextra -Isrc -g

SRC_DIR = src
SUB_DIR = src/subprog
BIN_DIR = bin

SUB_FILES = $(wildcard $(SUB_DIR)/*.c)

SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
MAIN_FILE = $(SRC_DIR)/main.c

SUB_TARGETS = $(patsubst $(SUB_DIR)/%.c, $(BIN_DIR)/%, $(SUB_FILES))
TARGET = $(BIN_DIR)/app

$(shell mkdir -p $(BIN_DIR))

all: $(TARGET)

$(TARGET): $(SRC_FILES) $(MAIN_FILE)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%: $(SUB_DIR)/%.c
	$(CC) $(CFLAGS) -o $@ $^

subprog: $(SUB_TARGETS)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/*