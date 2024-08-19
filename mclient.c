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
       if(x)
        printf("Recieved msg : %s\n", buff);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            break;
        }
    }
}

void writeThread(void* arg) {
    SOCKET sockfd = *((SOCKET*)arg);
    char buff[MAX];

    for (;;) {
        printf("Send : ");
        fgets(buff, sizeof(buff), stdin);
        send(sockfd, buff, strlen(buff), 0);

        if (strncmp(buff, "exit", 4) == 0) {
            printf("Client Exit...\n");
            break;
        }
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
        printf("socket creation failed...\n");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.137.117");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
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
