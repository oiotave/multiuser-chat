#!/bin/bash

# Versão super simples - só mostra o essencial

echo "=== TESTE SIMPLES MULTICLIENTE ==="

# Compilar
make server client

# Limpar anteriores
pkill -f "bin/server" 2>/dev/null || true
pkill -f "bin/client" 2>/dev/null || true

# Servidor com timeout
echo "Iniciando servidor (15 segundos)..."
timeout 15s ./bin/server &
SERVER_PID=$!

sleep 3

# Clientes rápidos
echo "Executando 3 clientes..."
for i in 1 2 3; do
    echo "Cliente $i" | timeout 8s ./bin/client &
    sleep 1
done

echo "Aguardando término..."
wait 2>/dev/null


echo "Teste completo!"

