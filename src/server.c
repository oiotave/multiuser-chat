#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/libtslog.h"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 8080

// Estrutura para armazenar informações dos clientes
typedef struct {
    int socket;
    struct sockaddr_in address;
    int id;
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Executa a difusão de mensagens enviadas pelos clientes
void broadcast_message(const char *message, int sender_id) {
    pthread_mutex_lock(&clients_mutex);
    
    // Só pode enviar uma mensagem por vez, para evitar conflito
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i] != NULL && clients[i]->id != sender_id) {
            if(send(clients[i]->socket, message, strlen(message), 0) < 0) {
                log_msg(ERROR, "Falha ao enviar mensagem para cliente %d", clients[i]->id);
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
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
            break;
        }
    }   
    pthread_mutex_unlock(&clients_mutex);
}

// Prepara a chegada de um novo cliente e as mensagens enviadas
void *handle_client(void *arg) {
    client_t *client = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    char client_name[50] = "Anonimo";
    
    // Permite ao cliente escolher um nome de usuário
    send(client->socket, "Digite seu nome: ", 18, 0);
    int name_size = recv(client->socket, buffer, sizeof(buffer) - 1, 0);
    
    // Faz o tratamento adequado da reposta do cliente
    if(name_size > 0) {
        buffer[name_size] = '\0';

        buffer[strcspn(buffer, "\n")] = 0;
        if(strlen(buffer) > 0) {
            strncpy(client_name, buffer, sizeof(client_name) - 1);
        }
    }
    char message[BUFFER_SIZE + 100];

    // Notifica a entrada do cliente no chat
    snprintf(message, sizeof(message), "[SERVIDOR] %s entrou no chat", client_name);
    log_msg(MSG, "Novo cliente: %s (ID %d)", client_name, client->id);
    broadcast_message(message, client->id);
    
    // Laço principal para recebimento e difusão de mensagens enviadas pelos clientes
    while(1) {
        int read_size = recv(client->socket, buffer, sizeof(buffer) - 1, 0);
        
        if(read_size <= 0) break;
        
        buffer[read_size] = '\0';
        buffer[strcspn(buffer, "\n")] = 0;
        
        // Prepara mensagem para broadcast e para o log apropriado  
        snprintf(message, sizeof(message), "%s: %s", client_name, buffer);
        log_msg(MSG, "Mensagem de %s: %s", client_name, buffer);
        broadcast_message(message, client->id);

        // Verifica o comando de saída
        if(strcmp(buffer, "/sair") == 0) break;
    }
    // Notifica a saída do cliente no chat
    snprintf(message, sizeof(message), "%s saiu do chat", client_name);
    broadcast_message(message, client->id);
    remove_client(client->id);
    
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;
    int client_count = 0;
    
    log_init(stdout);
    log_msg(MSG, "Iniciando servidor de chat multiusuario");
    
    // Cria um socket para o servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1) {
        log_msg(ERROR, "Falha ao criar socket do servidor");
        exit(EXIT_FAILURE);
    }
    
    // Configura o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Faz a conexão com o socket apropriado
    if(bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        log_msg(ERROR, "Falha no bind do socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Tenta fazer o servidor ouvir no socket
    if(listen(server_socket, MAX_CLIENTS) < 0) {
        log_msg(ERROR, "Falha no listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    log_msg(MSG, "Servidor ouvindo na porta %d", PORT);
    
    // Inicializa cada um dos cliente possíveis no vetor auxiliar
    for (int i = 0; i < MAX_CLIENTS; i++) clients[i] = NULL;
    
    // Aceita as conexões
    while(1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if(client_socket < 0) {
            log_msg(ERROR, "Falha ao aceitar conexão");
            continue;
        }
        pthread_mutex_lock(&clients_mutex);
        
        // Tenta encontrar um slot livre parao cliente novo, se houver
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
            continue;
        }
        // Cria um novo cliente no vetor de clientes
        client_t *new_client = malloc(sizeof(client_t));
        new_client->socket = client_socket;
        new_client->address = client_addr;
        new_client->id = ++client_count;
        
        clients[slot] = new_client;
        
        pthread_mutex_unlock(&clients_mutex);
        
        // Cria uma thread para tratar o cliente e suas mensagens
        if(pthread_create(&thread_id, NULL, handle_client, (void *)new_client) != 0) {
            log_msg(ERROR, "Falha ao criar thread para cliente");
            remove_client(new_client->id);
        }
        pthread_detach(thread_id);
    }
    close(server_socket);
    log_close();
    return 0;
}