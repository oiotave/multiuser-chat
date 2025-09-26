#include "../include/libtslog.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static FILE *log_output = NULL;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void log_init(FILE *output) { log_output = output; }

void log_msg(logtype_t level, const char *format, ...) {
    if(!log_output) return;
    
    pthread_mutex_lock(&mutex);
    
    // Define o tipo e a cor da mensagem
    const char *type;
    const char *color_code = "";
    const char *reset_code = "\033[0m";
    
    switch(level) {
        case MSG:
        type = "MSG";
        color_code = "\033[32m";  // Verde
        break;
        
        case WARNING:
        type = "WARNING";
        color_code = "\033[33m";  // Amarelo
        break;
        
        case ERROR:
        type = "ERROR";
        color_code = "\033[31m";  // Vermelho
        break;
        
        default:
        type = "UNKNOWN";
        color_code = "\033[37m";  // Branco
    }
    // Mostra o cabeçalho com a cor apropriada
    fprintf(log_output, "%s[%s] %s ", color_code, type, reset_code);
    
    va_list args;
    va_start(args, format);

    // Adiciona a mensagem passada no log
    vfprintf(log_output, format, args);
    va_end(args);
    
    fprintf(log_output, "\n");
    
    fflush(log_output);
    pthread_mutex_unlock(&mutex);
}

void log_close(void) {
    pthread_mutex_lock(&mutex);
    
    // Encerra apenas se o endpoint for um arquivo
    if(log_output && log_output != stdout && log_output != stderr) fclose(log_output);
    
    log_output = NULL;
    pthread_mutex_unlock(&mutex);
}