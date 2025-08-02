CC = gcc
CFLAGS = -Wall -Wextra -Isrc

SRC_DIR = src

SRC_FILES = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
MAIN_FILE = $(SRC_DIR)/main.c

TARGET = bin/app

$(shell mkdir -p bin)

all: $(TARGET)

$(TARGET): $(SRC_FILES) $(MAIN_FILE)
	$(CC) $(CFLAGS) -o $(TARGET) $^

.PHONY: clean
clean:
	rm -rf $(TARGET) $(TEST_TARGET)