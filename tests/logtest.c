// tests/test_logging.c
#include "../include/libtslog.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define THREAD_NUMBER 5

void* thread_function(void* arg) {
    int thread_id = *(int*) arg;
    
    log_msg(MSG, "Thread %d iniciou execucao", thread_id);
    
    // Simula algum processamento
    for(int i = 0; i < 3; i++) {
        log_msg(MSG, "Thread %d: enviando mensagem %d", thread_id, i + 1);
        usleep(100000); // 100ms
    }
    
    // Gera diferentes tipos de log baseado no ID
    for(int j = 0; j < 2; j++) {
        switch(thread_id % 3) {
            case 0:
            log_msg(WARNING, "Thread %d: Mensagem de atencao", thread_id);
            break;
            
            case 1:
            log_msg(ERROR, "Thread %d: Mensagem de erro", thread_id);
            break;
            
            case 2:
            log_msg(MSG, "Thread %d: operacao concluida com sucesso", thread_id);
            break;
        }
    }
    log_msg(MSG, "Thread %d finalizou", thread_id);
    return NULL;
}

void test_multithreaded() {
    pthread_t threads[5];
    int thread_ids[THREAD_NUMBER];
    
    for(int i = 0; i < THREAD_NUMBER; i++) thread_ids[i] = i + 1;

    log_msg(MSG, "Iniciando teste com %d threads concorrentes", THREAD_NUMBER);

    for(int i = 0; i < 5; i++) {
        if(pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]) != 0)
            log_msg(ERROR, "Falha ao criar thread %d", i);
    }
    for(int i = 0; i < 5; i++) pthread_join(threads[i], NULL);
    log_msg(MSG, "Todas as threads finalizaram");
}

int main() {
    printf("=== TESTES DE FUNCOES DE LOG ===\n\n");
    
    log_init(stdout);
    test_multithreaded();    
    return 0;
}