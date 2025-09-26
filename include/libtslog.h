#ifndef LIBTSLOG_H
#define LIBTSLOG_H

#include <stdio.h>
#include <pthread.h>
#include <time.h>

// Codifica o tipo de mensagem
typedef enum { MSG, ERROR, WARNING } logtype_t;

// Inicializa o log
void log_init(FILE*);

// Envia a mensagem no log
void log_msg(logtype_t, const char*, ...);

// Fecha o log
void log_close(void);

#endif