CC = gcc
CFLAGS = -Wall -Wextra -Isrc -g

SRC_DIR = src
BIN_DIR = bin
SERVER_DIR = $(SRC_DIR)/server
CLIENT_DIR = $(SRC_DIR)/client

SRC_SERVER = $(wildcard $(SRC_DIR)/*.c $(SERVER_DIR)/*.c)
SRC_CLIENT = $(wildcard $(SRC_DIR)/*.c $(CLIENT_DIR)/*.c)

SERVER_TARGET = $(BIN_DIR)/server_app
CLIENT_TARGET = $(BIN_DIR)/client_app

$(shell mkdir -p $(BIN_DIR))

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SRC_SERVER)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT_TARGET): $(SRC_CLIENT)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/*