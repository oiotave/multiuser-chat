#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H

#include <pthread.h>
#include <stdlib.h>

typedef struct {
    void **data;    // Vetor de ponteiros para cada elemento da fila
    int capacity;   // Capacidade máxima da fila
    int size;       // Número atual de elementos na fila
    int front;      // Índice do primeiro elemento
    int rear;       // Índice do último elemento

    pthread_mutex_t mutex;      // Mutex para exclusão mútua
    pthread_cond_t not_empty;   // Condicional para quando a fila não estiver vazia
    pthread_cond_t not_full;    // Condicional para quando a fila não estiver cheia
}   threadsafe_queue_t;

// Inicializa a fila thread_safe
void queue_init(threadsafe_queue_t *queue, int capacity);

// Destrói a fila e libera recursos
void queue_destroy(threadsafe_queue_t *queue);

// Adiciona um elemento na fila
void queue_push(threadsafe_queue_t *queue, void *item);

// Remove um elemento da fila
void *queue_pop(threadsafe_queue_t *queue);

// Verifica se a fila está vazia
int queue_is_empty(threadsafe_queue_t *queue);

#endif

