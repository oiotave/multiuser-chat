#include "threadsafe_queue.h"
#include <stdio.h>

// Inicializa cada campo do monitor
void queue_init(threadsafe_queue_t *queue, int capacity) {
    queue->data = malloc(sizeof(void*) * capacity);
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

// Libera recursos que tenham sido alocados dinamicamente
void queue_destroy(threadsafe_queue_t *queue) {
    free(queue->data);
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
}

void queue_push(threadsafe_queue_t *queue, void *item) {
    pthread_mutex_lock(&queue->mutex);
    
    // Espera enquanto a fila estiver cheia
    while(queue->size == queue->capacity) pthread_cond_wait(&queue->not_full, &queue->mutex);
    
    // Insere elemento na última posição
    queue->data[queue->rear] = item;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;
    
    // Sinaliza que a fila não está mais cheia
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->mutex);
}

void *queue_pop(threadsafe_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    
    // Espera enquanto a fila estiver vazia
    while(queue->size == 0) pthread_cond_wait(&queue->not_empty, &queue->mutex);
    
    // Retira elemento da primeira posição
    void *item = queue->data[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    
    // Sinaliza que a fila não está mais vazia
    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->mutex);
    
    return item;
}

// Verifica se a fila está vazia
int queue_is_empty(threadsafe_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    int empty = (queue->size == 0);
    pthread_mutex_unlock(&queue->mutex);
    return empty;
}

