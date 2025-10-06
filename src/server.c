#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/libtslog.h"
#include "../include/threadsafe_queue.h"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 8080
#define QUEUE_CAPACITY 100


typedef struct { // Estrutura para mensagem
    char content[BUFFER_SIZE];
    int sender_id;
}   message_t;

typedef struct { // Estrutura para cliente
    int socket;
    struct sockaddr_in address;
    int id;
}   client_t;

// Semáforo para indicar os slots de cliente disponíveis
sem_t available_slots;

// Vetor de clientes
client_t *clients[MAX_CLIENTS];

// Mutex para tratar operações de clientes com concorência
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Declaração do monitor
threadsafe_queue_t message_queue;

// Função de cada thread para processar mensagens da fila
void *broadcast_worker() {
    while(1) {
        message_t *msg = queue_pop(&message_queue);
        pthread_mutex_lock(&clients_mutex);
        
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(clients[i] != NULL && clients[i]->id != msg->sender_id) {
                if(send(clients[i]->socket, msg->content, strlen(msg->content), 0) < 0) {
                    log_msg(ERROR, "Falha ao enviar mensagem para cliente %d", clients[i]->id);
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
        free(msg);
    }
    return NULL;
}

// Função para enviar mensagem do sistema (sem sender_id específico)
void broadcast_system_message(const char *message) {
    message_t *msg = malloc(sizeof(message_t));
    
    strncpy(msg->content, message, BUFFER_SIZE - 1);
    msg->content[BUFFER_SIZE - 1] = '\0';
    msg->sender_id = -1; // Valor -1 indica mensagem do sistema

    queue_push(&message_queue, msg);
}

// Remove um cliente quando ele desconecta
void remove_client(int client_id) {
    pthread_mutex_lock(&clients_mutex);
    
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i] != NULL && clients[i]->id == client_id) {
            log_msg(MSG, "Cliente %d desconectado", client_id);
            close(clients[i]->socket);
            free(clients[i]);
            clients[i] = NULL;
            
            // Libera informação no semáforo
            sem_post(&available_slots);
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Manipula cada cliente em thread separada
void *handle_client(void *arg) {
    client_t *client = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    char client_name[50] = "Anonimo";

    // Solicita e recebe nome do cliente
    send(client->socket, "Digite seu nome: ", 17, 0);
    int name_size = recv(client->socket, buffer, sizeof(buffer) - 1, 0);
    
    // Faz o tratamento adequado do nome do cliente
    if(name_size > 0) {
        buffer[name_size] = '\0';
        buffer[strcspn(buffer, "\n")] = 0;
        
        if(strlen(buffer) > 0) strncpy(client_name, buffer, sizeof(client_name) - 1);
    }

    // Notifica entrada do cliente
    char message[BUFFER_SIZE + 100];
    snprintf(message, sizeof(message), "[SERVIDOR] %s entrou no chat", client_name);
    log_msg(MSG, "Novo cliente: %s (ID %d)", client_name, client->id);
    broadcast_system_message(message);

    // Loop principal de mensagens
    while(1) {
        int read_size = recv(client->socket, buffer, sizeof(buffer) - 1, 0);
        
        if (read_size <= 0) break;
        
        buffer[read_size] = '\0';
        buffer[strcspn(buffer, "\n")] = 0;

        // Envia mensagem para a fila
        message_t *new_msg = malloc(sizeof(message_t));
        snprintf(new_msg->content, sizeof(new_msg->content), "%s: %s", client_name, buffer);
        new_msg->sender_id = client->id;
        queue_push(&message_queue, new_msg);
        
        log_msg(MSG, "Mensagem de %s enfileirada: %s", client_name, buffer);

        // Verifica se cliente deseja sair
        if(strcmp(buffer, "/sair") == 0) break;
    }

    // Notifica saída do cliente
    snprintf(message, sizeof(message), "[SERVIDOR] %s saiu do chat", client_name);
    broadcast_system_message(message);
    remove_client(client->id);
    
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t broadcast_thread, client_thread;
    int client_count = 0;

    // Inicializa semáforo e monitor
    sem_init(&available_slots, 0, MAX_CLIENTS);
    queue_init(&message_queue, QUEUE_CAPACITY);

    log_init(stdout);
    log_msg(MSG, "Iniciando servidor de chat multiusuario");

    // Cria socket do servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1) {
        log_msg(ERROR, "Falha ao criar socket do servidor");
        exit(EXIT_FAILURE);
    }

    // Configura endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Executa o bind e o listen do socket
    if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_msg(ERROR, "Falha no bind do socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    if(listen(server_socket, MAX_CLIENTS) < 0) {
        log_msg(ERROR, "Falha no listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    log_msg(MSG, "Servidor ouvindo na porta %d", PORT);

    // Inicializa array de clientes
    for(int i = 0; i < MAX_CLIENTS; i++) clients[i] = NULL;

    // Cria thread para broadcast
    if(pthread_create(&broadcast_thread, NULL, broadcast_worker, NULL) != 0) {
        log_msg(ERROR, "Falha ao criar thread de broadcast");
        exit(EXIT_FAILURE);
    }
    pthread_detach(broadcast_thread);

    // Aceita conexões
    while(1) {
        // Reduz recursos de slot do semáforo
        sem_wait(&available_slots);
        
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if(client_socket < 0) {
            sem_post(&available_slots);
            log_msg(ERROR, "Falha ao aceitar conexao");
            continue;
        }
        pthread_mutex_lock(&clients_mutex);

        // Encontra um slot livre para o cliente, se possível
        int slot = -1;
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(clients[i] == NULL) {
                slot = i;
                break;
            }
        }
        if(slot == -1) {
            log_msg(WARNING, "Numero maximo de clientes atingido");
            close(client_socket);
            pthread_mutex_unlock(&clients_mutex);
            sem_post(&available_slots);
            continue;
        }

        // Cria um novo cliente
        client_t *new_client = malloc(sizeof(client_t));
        new_client->socket = client_socket;
        new_client->address = client_addr;
        new_client->id = ++client_count;
        clients[slot] = new_client;

        pthread_mutex_unlock(&clients_mutex);

        // Cria uma thread para gerenciar operações do novo cliente
        if(pthread_create(&client_thread, NULL, handle_client, (void *)new_client) != 0) {
            log_msg(ERROR, "Falha ao criar thread para cliente");
            remove_client(new_client->id);
        }
        pthread_detach(client_thread);
    }
    // Fecha e finaliza cada estrutura e conexão
    close(server_socket);
    queue_destroy(&message_queue);
    sem_destroy(&available_slots);
    log_close();
    return 0;

}

