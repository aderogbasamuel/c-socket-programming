#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
    int status, valread, client_fd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client";
    char buffer[1024] = { 0 };
    int byteReceived;
    int totalRecieved = 0;
long fileSize = 0;
    FILE *fp = fopen("Received files/Receieved.png", "wb");
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
 if(fp == NULL) {
        perror("Error opening file");
        return 1;
    };
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "172.26.191.59", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((status
         = connect(client_fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
  
    // subtract 1 for the null
    // terminator at the end
    
    
    send(client_fd, hello, strlen(hello), 0);
    valread = read(client_fd, buffer,
                   1024 - 1); 
    
    printf("%s\n", buffer);
    recv(client_fd, &fileSize, sizeof(fileSize), 0);
    while(totalRecieved < fileSize){
        if((byteReceived= recv(client_fd, buffer, sizeof(buffer), 0)) > 0){
            fwrite(buffer, 1, byteReceived, fp);
            totalRecieved += byteReceived;
            printf("Received %d bytes, total received: %d bytes\n", byteReceived, totalRecieved);
        }
    }
    printf("ALready sent the file bro!!\n");
    

    // closing the connected socket
    close(client_fd);
    fclose(fp);
    return 0;
}