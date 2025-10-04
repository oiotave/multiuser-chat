# Multiuser Chat - Sistema de Log (libtslog)

Essa é a segunda entrega do projeto final da disciplina de Linguagem de Programação II, que busca implementar um chat multiusuário por meio de sockets TCP usando princípios de concorrência. Dessa vez, além da criação de uma rotina de logging thread-safe, a `libtslog`, foram implementadas as rotinas que definem o servidor para o chat e para os clientes/usuários (cada um gerenciado por uma thread diferente). Para checar a funcionalidade dessas atualizações, foi criado um script de teste no diretório `/scripts`.

## Estrutura do Projeto

Para essa entrega foram implementados os seguintes códigos-fonte:

### Biblioteca de Logging

`libtslog`: cabeçalho com definições de log (`Dinclude/libtslog.h`) e implementação de funções de log (`src/libtslog.c`);

### Testes de Log

`logtest`: código-fonte com testes de funções de log (tests/logtest.c);

### Sistema de Chat

`server`: servidor multithreaded que aceita múltiplos clientes e retransmite mensagens (`src/server.c`);

`client`: cliente de chat com interface de linha de comando para enviar e receber mensagens (`src/client.c`);

`test_multiclient`: scripts de teste automatizado para verificar a interação entre múltiplos clientes (`scripts/test_multiclient.sh`).

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
```bash
make test-multi
```
**Observação:** rode `make test-multi` em outro terminal.
