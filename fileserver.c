#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define MAX_CLIENTS 10
#define MAX_MSG 1024
#define PORT 8080
#define SA struct sockaddr

SOCKET client_fds[MAX_CLIENTS];
int client_count = 0;

DWORD WINAPI readThread(LPVOID lpParam) {
    SOCKET sockfd = *((SOCKET *)lpParam);
    char filename[MAX_MSG];
    char buff[MAX_MSG];
    int n;

    // Receive the filename from the client
    n = recv(sockfd, filename, sizeof(filename), 0);
    if (n <= 0) {
        printf("Error receiving filename\n");
        closesocket(sockfd);
        return 0;
    }

    // Null-terminate the filename
    filename[n] = '\0';
    printf("Received filename: %s\n", filename);

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        closesocket(sockfd);
        return 0;
    }

    // Read the file content into buffer and send it to the client
    while ((n = fread(buff, 1, sizeof(buff), file)) > 0) {
        if (send(sockfd, buff, n, 0) != n) {
            printf("Error sending file\n");
            fclose(file);
            closesocket(sockfd);
            return 0;
        }
    }

    fclose(file);
    closesocket(sockfd);

    return 0;
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

        // Create a thread to handle communication with the client
        threads[client_count] = CreateThread(NULL, 0, readThread, &connfd, 0, &threadID);
        if (threads[client_count] == NULL) {
            printf("Error creating thread\n");
            exit(EXIT_FAILURE);
        }

        client_count++;
    }

    // Close server socket
    closesocket(sockfd);

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
