# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin

SRC = $(SRC_DIR)/libtslog.c
TEST_SRC = $(TEST_DIR)/logtest.c
TARGET = $(BIN_DIR)/logtest

# Cores para output
GREEN = \033[0;32m
RED = \033[0;31m
NC = \033[0m

all: $(TARGET)

$(TARGET): $(SRC) $(TEST_SRC)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(TEST_SRC)
	@echo "$(GREEN) Compilação concluída$(NC)"

run: $(TARGET)
	@./$(TARGET)

clean:
	@rm -rf $(BIN_DIR) *.log

.PHONY: all run valgrind clean debug

