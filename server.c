#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define MAX_CLIENTS 10
#define MAX_MSG 80
#define PORT 8080
#define SA struct sockaddr

SOCKET client_fds[MAX_CLIENTS];
int client_count = 0;

DWORD WINAPI func(LPVOID lpParam) {
    SOCKET sockfd = *((SOCKET *)lpParam);
    char buff[MAX_MSG];
    int n;

    while (1) {
        memset(buff, 0, sizeof(buff));
        n = recv(sockfd, buff, sizeof(buff), 0);
        if (n <= 0) {
            printf("Client disconnected\n");
            closesocket(sockfd);

            // Remove the client from the list
            for (int i = 0; i < client_count; i++) {
                if (client_fds[i] == sockfd) {
                    for (int j = i; j < client_count - 1; j++) {
                        client_fds[j] = client_fds[j + 1];
                    }
                    client_count--;
                    break;
                }
            }
            return 0;
        }

        // Broadcast message to all clients
        for (int i = 0; i < client_count; i++) {
            if (client_fds[i] != sockfd) {
                send(client_fds[i], buff, n, 0);
            }
        }

        // Check for exit command
        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            closesocket(sockfd);

            // Remove the client from the list
            for (int i = 0; i < client_count; i++) {
                if (client_fds[i] == sockfd) {
                    for (int j = i; j < client_count - 1; j++) {
                        client_fds[j] = client_fds[j + 1];
                    }
                    client_count--;
                    break;
                }
            }
            return 0;
        }
    }
}

int main() {
    WSADATA wsa;
    SOCKET sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    HANDLE threads[MAX_CLIENTS];
    DWORD threadID;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("socket creation failed...\n");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to all available interfaces
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (SA *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        printf("socket bind failed...\n");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) == SOCKET_ERROR) {
        printf("Listen failed...\n");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on IP ..\n");

    int len = sizeof(cli);

    // Accept connections in a loop
    while (1) {
        // Accept a new client connection
        connfd = accept(sockfd, (SA *)&cli, &len);
        if (connfd == INVALID_SOCKET) {
            printf("server accept failed...\n");
            exit(EXIT_FAILURE);
        }

        printf("server accept the client...\n");

        // Add the new client to the list
        client_fds[client_count++] = connfd;

        // Create a thread to handle communication with the client
        threads[client_count - 1] = CreateThread(NULL, 0, func, &connfd, 0, &threadID);
        if (threads[client_count - 1] == NULL) {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }
    }

    // Close server socket
    closesocket(sockfd);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}