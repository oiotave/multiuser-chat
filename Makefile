CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude -Wno-format-truncation
SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin
SCRIPTS_DIR = scripts

# Fontes
LIB_SRC = $(SRC_DIR)/libtslog.c
SERVER_SRC = $(SRC_DIR)/server.c $(SRC_DIR)/threadsafe_queue.c
CLIENT_SRC = $(SRC_DIR)/client.c
TEST_SRC = $(TEST_DIR)/logtest.c

# Binários
SERVER_TARGET = $(BIN_DIR)/server
CLIENT_TARGET = $(BIN_DIR)/client
TEST_TARGET = $(BIN_DIR)/logtest

# Cores
GREEN = \033[0;32m
RED = \033[0;31m
YELLOW = \033[0;33m
BLUE = \033[0;34m
NC = \033[0m

all: server client $(TEST_TARGET)

$(TEST_TARGET): $(LIB_SRC) $(TEST_SRC)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $(TEST_TARGET) $(LIB_SRC) $(TEST_SRC)
	@echo "$(GREEN)✓ Testes de log compilados$(NC)"

server: $(LIB_SRC) $(SERVER_SRC)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $(SERVER_TARGET) $(LIB_SRC) $(SERVER_SRC)
	@echo "$(GREEN)✓ Servidor compilado$(NC)"

client: $(LIB_SRC) $(CLIENT_SRC)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $(CLIENT_TARGET) $(LIB_SRC) $(CLIENT_SRC)
	@echo "$(GREEN)✓ Cliente compilado$(NC)"

# Regras de execução
run-test: $(TEST_TARGET)
	@./$(TEST_TARGET)

run-server: server
	@echo "$(BLUE)Iniciando servidor...$(NC)"
	@./$(SERVER_TARGET)

run-client: client
	@echo "$(BLUE)Iniciando cliente...$(NC)"
	@./$(CLIENT_TARGET)

# Teste com múltiplos clientes
test-multi: server client
	@echo "$(YELLOW)Testando múltiplos clientes...$(NC)"
	@if [ -f "$(SCRIPTS_DIR)/test_multiclient.sh" ]; then \
		chmod +x $(SCRIPTS_DIR)/test_multiclient.sh; \
		$(SCRIPTS_DIR)/test_multiclient.sh; \
	else \
		echo "$(RED)Script de teste não encontrado: $(SCRIPTS_DIR)/test_multiclient.sh$(NC)"; \
		echo "$(YELLOW)Criando script básico de teste...$(NC)"; \
		@echo "#!/bin/bash" > $(SCRIPTS_DIR)/test_multiclient.sh; \
		@echo "echo 'Script de teste multicliente - versão básica'" >> $(SCRIPTS_DIR)/test_multiclient.sh; \
		@echo "echo 'Servidor compilado, mas script de teste precisa ser implementado'" >> $(SCRIPTS_DIR)/test_multiclient.sh; \
		chmod +x $(SCRIPTS_DIR)/test_multiclient.sh; \
		$(SCRIPTS_DIR)/test_multiclient.sh; \
	fi

# Limpeza
clean:
	@rm -rf $(BIN_DIR) *.log
	@echo "$(RED)✓ Arquivos de compilação removidos$(NC)"

.PHONY: all run-test run-server run-client test-multi clean help

