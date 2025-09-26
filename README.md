# Multiuser Chat - Sistema de Log (libtslog)

Essa é a primeira entrega do projeto final da disciplina de Linguagem de Programação II, que busca implementar um chat multiusuário por meio de sockets TCP usando princípios de concorrência. Por ora, foi feita a demarcação de cabeçalhos importantes para o projeto e a criação de uma rotina de logging thread-safe, a `libtslog`. Para checar a funcionalidade dos logs, foi feito um arquivo de teste simples no diretório `/tests`.

## Visão Geral

A `libtslog` é uma biblioteca de logging leve e eficiente desenvolvida para sistemas multithreaded. Oferece:

- **Thread-safe**: Logs seguros para uso concorrente com mutex
- **Código de cores**: Diferentes cores para cada tipo de mensagem no terminal
- **Múltiplos níveis**: MSG, WARNING e ERROR
- **Flexível**: Suporte a saída para stdout, stderr ou arquivos

##  Estrutura do Projeto

Para essa entrega foram implementados os seguintes códigos-fonte:

- `libtslog`: cabeçalho com definições de log (`include/libtslog.h`) e implementação de funções de log (`src/libtslog.c`);

- `logtest`: código-fonte com testes de funções de log (`tests/logtest.c`)

Outros códigos foram declarados, mas suas implementações serão reservadas para futuras entregas. Além disso, diagramas de descrição do sistema podem ser encontrados no diretório `docs`.

## Instruções de compilação e execução

Esse projeto possui um Makefile para automatizar a compilação. Certifique-se que seu terminal tem suporte para o comando `make` e execute os seguinte comandos para compilar e executar:

```bash
make
./bin/logtest
```