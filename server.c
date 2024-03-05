#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024
int client_count = 0;
int semaphore = 1;

void signal(int *semaphore) {
    semaphore++;
}

void wait(int *semaphore) {
    while (semaphore <= 0);
    semaphore--;
}

void *client_handler(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while(1)
    {
        
        // Receive IP name from client
        memset(buffer, 0, sizeof(buffer));
        if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
            perror("Error receiving data from client");
            continue;
        }
        // Resolve IP name to IP address
        buffer[strlen(buffer)-2] = '\0'; // Remove newline character from buffer
        if (strcmp(buffer, "exit") == 0) {
            printf("Client disconnected\n");
            close(client_socket);
            wait(&semaphore);
            client_count--;
            signal(&semaphore);
            printf("Total clients: %d\n", client_count);
            pthread_exit(NULL);
        }
        
        struct hostent *host = gethostbyname(buffer);
        if (host == NULL) {
            perror("Error resolving IP name");
            continue;
        }

        // Convert IP address to string
        char *IP;
        IP = inet_ntoa(*((struct in_addr *)host->h_addr_list[0]));

        IP[strlen(IP)] = '\n';

        // Send IP address back to client
        if (send(client_socket, IP, strlen(IP), 0) < 0){
            perror("Error sending data to client");
            continue;
        }

    }

}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    pthread_t thread_id;

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8000);

    // Bind server socket to address
    while (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding server socket");
        continue;
    }
    system("clear");

    // Listen for connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8000...\n");

    while (1) {
        // Accept client connection
        socklen_t client_address_length = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_length);
        if (client_socket < 0) {
            perror("Error accepting client connection");
            continue;
        }

        // Create a new thread to handle the client
        if (pthread_create(&thread_id, NULL, client_handler, (void *)&client_socket) < 0) {
            perror("Error creating thread");
            close(client_socket);
            continue;
        }

        // Detach the thread to allow it to run independently
        pthread_detach(thread_id);

        // Increment client count
        client_count++;

        printf("Client connected. Total clients: %d\n", client_count);
    }

    close(server_socket);
    return 0;
}