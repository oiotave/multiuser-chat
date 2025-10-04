#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/libtslog.h"

#define BUFFER_SIZE 1024
#define SERVER_PORT 8080
#define SERVER_IP "127.0.0.1" // Usa o localhost para simular conexão em rede

int client_socket;
int running = 1;

// Recebe as mensagens de cada cliente/thread 
void* receive_messages() {
    char buffer[BUFFER_SIZE];
    
    while(running) {
        int read_size = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if(read_size <= 0) {
            log_msg(ERROR, "Conexao com servidor perdida");
            running = 0;
            break;
        }
        buffer[read_size] = '\0';
        printf("\r%s\n> ", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
    pthread_t thread_client;
    char buffer[BUFFER_SIZE];
    
    // Cria um ponto de log no terminal
    log_init(stdout);
    
    // Cria um socket para o cliente
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1) {
        log_msg(ERROR, "Falha ao criar socket do cliente");
        exit(EXIT_FAILURE);
    }
    
    // Configura endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        log_msg(ERROR, "Endereco de servidor invalido");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    // Conecta cliente ao servidor
    if(connect(client_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        log_msg(ERROR, "Falha ao conectar com servidor");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    log_msg(MSG, "Conectado ao servidor %s:%d", SERVER_IP, SERVER_PORT);
    printf("Digite mensagens (digite '/sair' para sair):\n> ");
    
    // Cria uma thread para receber mensagens
    if(pthread_create(&thread_client, NULL, receive_messages, NULL) != 0) {
        log_msg(ERROR, "Falha ao criar thread de recebimento");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    
    // Laço para o envio de mensagens pelo cliente
    while(running) {
        if(fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;
            
            // Envia uma mensagem para o servidor
            if(strlen(buffer) > 0) {
                if(send(client_socket, buffer, strlen(buffer), 0) < 0) {
                    log_msg(ERROR, "Falha ao enviar mensagem");
                    break;
                }
                // Verifica se o cliente deseja desconectar
                if(strcmp(buffer, "/sair") == 0) {
                    running = 0;
                    break;
                }
            }
            printf("> ");
            fflush(stdout);
        }
    }
    running = 0;
    pthread_join(thread_client, NULL);
    close(client_socket);
    
    log_msg(MSG, "Cliente encerrado");
    log_close();
    return 0;
}