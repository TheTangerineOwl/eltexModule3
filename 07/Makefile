CC = gcc
CFLAGS = -Wall -Wextra -Isrc -Irt -g

SRC_DIR = src
BIN_DIR = bin

SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
MAIN_FILE = $(SRC_DIR)/main.c

TARGET = $(BIN_DIR)/app

$(shell mkdir -p $(BIN_DIR))

all: $(TARGET)

$(TARGET): $(SRC_FILES) $(MAIN_FILE)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/*