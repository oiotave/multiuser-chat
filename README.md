# Multiuser Chat - Sistema de Log (libtslog)

Essa é a terceira entrega do projeto final da disciplina de Linguagem de Programação II, que busca implementar um chat multiusuário por meio de sockets TCP usando princípios de concorrência. Dessa vez, encontra-se a aplicação completa com rotina de logging thread-safe, rotinas de servidor para o chat e para os clientes/usuários (cada um gerenciado por uma thread diferente) e uma rotina para monitor (fila com concorrência).

## Estrutura do Projeto

Para essa entrega foram implementados os seguintes códigos-fonte:

### Biblioteca de Logging

`libtslog`: cabeçalho com definições de log (`Dinclude/libtslog.h`) e implementação de funções de log (`src/libtslog.c`);

### Testes de Log

`logtest`: código-fonte com testes de funções de log (tests/logtest.c);

### Sistema de Chat

`server`: servidor multithreaded que aceita múltiplos clientes e retransmite mensagens (`src/server.c`);

`client`: cliente de chat com interface de linha de comando para enviar e receber mensagens (`src/client.c`);

`threadsafe_queue`: monitor thread-safe para fila de mensagens com sincronização por mutex e condvar (`include/threadsafe_queue.h, src/threadsafe_queue.c`);

`test_multiclient`: scripts de teste automatizado para verificar a interação entre múltiplos clientes (`scripts/test_multiclient.sh`).

### Outros

Outra rotina de teste continua disponível no diretório `/tests`. Os fluxos de funcionamento vistos na entrega 1 encontram-se no diretório `/fluxos`. O relatório de análise de qualidade, o digrama servidor-cliente e o mapeamento dos requisitos pedidos para essa entrega encontram todos no diretório `/docs`.

## Funcionalidades do Sistema de Chat

1. Servidor (`src/server.c`)

- Arquitetura multithread: Cada cliente atendido por thread dedicada
- Broadcast automático: Mensagens redistribuídas para todos os clientes
- Gerenciamento de conexões: Aceita até 10 clientes simultaneamente
- Log integrado: Todas as atividades registradas com libtslog
- Porta dinâmica: Tenta portas alternativas automaticamente se 8080 ocupada

2. Cliente (`src/client.c`)

- Interface simples: CLI fácil de usar
- Recebimento assíncrono: Mensagens recebidas em tempo real via thread separada
- Comandos especiais: /sair para desconexão graciosa
- Feedback visual: Confirmação de envio/recebimento

3. Monitor com Fila Thread-Safe (`threadsafe_queue.h/c`)

- Monitor implementado: Encapsula dados compartilhados com sincronização
- Operações atômicas: queue_push e queue_pop com exclusão mútua automática
- Sincronização eficiente: Uso de condition variables para bloqueio sem consumo de CPU
- Fila circular: Implementação eficiente com capacidade fixa
- Padrão produtor-consumidor: Suporte nativo para múltiplos produtores e consumidores

## Instruções de compilação e execução

Esse projeto possui um Makefile para automatizar a compilação. Certifique-se que seu terminal tem suporte para o comando `make` e execute os seguinte comandos para compilar e executar:

### Compilar todo o sistema
```bash
make all
```

### Executar o servidor
```bash
make run-server
```

### Executar o cliente
```bash
make run-client
```

**Observação:** rode `make run-client` em outro terminal, e repita o comando para cada cliente em um terminal diferente.

### Executar o teste autmático de clientes
Se você só quiser testar a funcionalidade básica de conexão entre servidor e cliente, rode o comando abaixo:

```bash
make test-multi
```
**Observação:** rode `make test-multi` em outro terminal, e certifique-se que não há outro servidor rodando na mesma porta, uma vez que `make test-multi` inicia o servidor por conta própria.
