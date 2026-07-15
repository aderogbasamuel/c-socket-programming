#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080

static void print_wsa_error(const char* message, int error_code)
{
    fprintf(stderr, "%s: %d\n", message, error_code);
}

int main(void)
{
    WSADATA wsaData;
    SOCKET client_fd = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    const char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    int byteReceived = 0;
    int totalRecieved = 0;
    long fileSize = 0;
    FILE* fp = NULL;
    int status = 0;
    int valread = 0;
    int wsaResult = 0;

    wsaResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaResult != 0) {
        print_wsa_error("WSAStartup failed", wsaResult);
        return -1;
    }

    fp = fopen("Received files\\Received.png", "wb");
    if (fp == NULL) {
        perror("Error opening file");
        WSACleanup();
        return 1;
    }

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == INVALID_SOCKET) {
        print_wsa_error("Socket creation error", WSAGetLastError());
        fclose(fp);
        WSACleanup();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "172.26.191.59", &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address/ Address not supported\n");
        closesocket(client_fd);
        fclose(fp);
        WSACleanup();
        return -1;
    }

    status = connect(client_fd, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (status == SOCKET_ERROR) {
        print_wsa_error("Connection Failed", WSAGetLastError());
        closesocket(client_fd);
        fclose(fp);
        WSACleanup();
        return -1;
    }

    status = send(client_fd, hello, (int)strlen(hello), 0);
    if (status == SOCKET_ERROR) {
        print_wsa_error("Send failed", WSAGetLastError());
        closesocket(client_fd);
        fclose(fp);
        WSACleanup();
        return -1;
    }

    valread = recv(client_fd, buffer, (int)sizeof(buffer) - 1, 0);
    if (valread == SOCKET_ERROR) {
        print_wsa_error("Receive failed", WSAGetLastError());
        closesocket(client_fd);
        fclose(fp);
        WSACleanup();
        return -1;
    }

    if (valread > 0) {
        buffer[valread] = '\0';
        printf("%s\n", buffer);
    }

    status = recv(client_fd, (char*)&fileSize, sizeof(fileSize), 0);
    if (status == SOCKET_ERROR) {
        print_wsa_error("Receive failed", WSAGetLastError());
        closesocket(client_fd);
        fclose(fp);
        WSACleanup();
        return -1;
    }

    while (totalRecieved < fileSize) {
        byteReceived = recv(client_fd, buffer, (int)sizeof(buffer), 0);
        if (byteReceived > 0) {
            fwrite(buffer, 1, byteReceived, fp);
            totalRecieved += byteReceived;
            printf("Received %d bytes, total received: %d bytes\n", byteReceived, totalRecieved);
        } else if (byteReceived == SOCKET_ERROR) {
            print_wsa_error("Receive failed", WSAGetLastError());
            break;
        } else {
            break;
        }
    }

    printf("ALready sent the file bro!!\n");

    if (closesocket(client_fd) == SOCKET_ERROR) {
        print_wsa_error("closesocket failed", WSAGetLastError());
    }

    if (fclose(fp) != 0) {
        perror("Error closing file");
    }

    if (WSACleanup() != 0) {
        print_wsa_error("WSACleanup failed", WSAGetLastError());
    }

    return 0;
}