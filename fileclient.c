#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void readThread(void* arg) {
    SOCKET sockfd = *((SOCKET*)arg);
    char buff[MAX];
    int n;

    for (;;) {
        memset(buff, 0, sizeof(buff));
        int x = recv(sockfd, buff, sizeof(buff), 0);
        
        if (x > 0) {
            printf("Received message: %s\n", buff);
        }

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Server closed the connection...\n");
            break;
        }
    }
}

void writeThread(void* arg) {
    SOCKET sockfd = *((SOCKET*)arg);
    char buff[MAX];
    int n;

    for (;;) {
        printf("Enter the file name to read: ");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = '\0';  // Remove newline character

        // Send the file name to the server
        send(sockfd, buff, strlen(buff), 0);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Closing the connection...\n");
            break;
        }

        // Wait for server's responsefdskl
        memset(buff, 0, sizeof(buff));
        n = recv(sockfd, buff, sizeof(buff), 0);
        if (n > 0) {
            printf("Received file content:\n%s\n", buff);
        } else {
            printf("Error receiving file content.\n");
            break;
        }

        // Writing the received file content to a new file
        printf("Enter the file name to write the content: ");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = '\0';  // Remove newline character

        FILE* file = fopen(buff, "w");
        if (file == NULL) {
            printf("Error opening file for writing.\n");
            break;
        }

        fputs(buff, file);
        fclose(file);
    }
}

int main() {
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in servaddr;
    HANDLE readThreadHandle, writeThreadHandle;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed...\n");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.1.1.0"); // Change to the server's IP address
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        closesocket(sockfd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Create threads for reading and writing
    readThreadHandle = (HANDLE)_beginthread(readThread, 0, &sockfd);
    writeThreadHandle = (HANDLE)_beginthread(writeThread, 0, &sockfd);

    // Wait for threads to finish
    WaitForSingleObject(readThreadHandle, INFINITE);
    WaitForSingleObject(writeThreadHandle, INFINITE);

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
