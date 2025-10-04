# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iinclude
SRC_DIR = src
TEST_DIR = tests
BIN_DIR = bin
SCRIPTS_DIR = scripts

# Fontes
LIB_SRC = $(SRC_DIR)/libtslog.c
SERVER_SRC = $(SRC_DIR)/server.c
CLIENT_SRC = $(SRC_DIR)/client.c
TEST_SRC = $(TEST_DIR)/logtest.c

# Binários
SERVER_TARGET = $(BIN_DIR)/server
CLIENT_TARGET = $(BIN_DIR)/client
TEST_TARGET = $(BIN_DIR)/logtest

# Cores para output
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
	@chmod +x $(SCRIPTS_DIR)/test_multiclient.sh
	@$(SCRIPTS_DIR)/test_multiclient.sh

# Debug
debug: CFLAGS += -g -DDEBUG
debug: all

# Limpeza
clean:
	@rm -rf $(BIN_DIR) *.log
	@echo "$(RED)✓ Arquivos de compilação removidos$(NC)"

# Ajuda
help:
	@echo "$(BLUE)=== Comandos disponíveis ===$(NC)"
	@echo "$(GREEN)all$(NC)        - Compila tudo (servidor, cliente, testes)"
	@echo "$(GREEN)server$(NC)     - Compila apenas o servidor"
	@echo "$(GREEN)client$(NC)     - Compila apenas o cliente"
	@echo "$(GREEN)run-server$(NC) - Executa o servidor"
	@echo "$(GREEN)run-client$(NC) - Executa o cliente"
	@echo "$(GREEN)run-test$(NC)   - Executa testes de log"
	@echo "$(GREEN)test-multi$(NC) - Testa múltiplos clientes simultaneamente"
	@echo "$(GREEN)test-network$(NC) - Teste básico de rede"
	@echo "$(GREEN)valgrind-*$(NC) - Executa com valgrind para detecção de leaks"
	@echo "$(GREEN)debug$(NC)      - Compila com símbolos de debug"
	@echo "$(GREEN)clean$(NC)      - Limpa arquivos compilados"

.PHONY: all run-test run-server run-client test-multi test-network \
        valgrind-test valgrind-server valgrind-client debug clean help

