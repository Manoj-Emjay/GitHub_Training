CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Werror -Isrc
BIN_DIR := bin

CORE_SRC := src/session_manager.c src/response_builder.c src/uds_handler.c
APP_SRC  := $(CORE_SRC) src/main.c

.PHONY: all app test clean

all: app test

app: $(BIN_DIR)/diagnostic_session_manager

$(BIN_DIR)/diagnostic_session_manager: $(APP_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(APP_SRC)

test: $(BIN_DIR)/test_session_manager $(BIN_DIR)/test_response_builder $(BIN_DIR)/test_uds_handler
	./$(BIN_DIR)/test_session_manager
	./$(BIN_DIR)/test_response_builder
	./$(BIN_DIR)/test_uds_handler

$(BIN_DIR)/test_session_manager: tests/test_session_manager.c src/session_manager.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_response_builder: tests/test_response_builder.c src/response_builder.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/test_uds_handler: tests/test_uds_handler.c src/uds_handler.c src/session_manager.c src/response_builder.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(BIN_DIR)
