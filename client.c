#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <port_number> <url>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    char *url = argv[2];

    // Create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Failed to create socket");
        return 1;
    // Create server address structure
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    // Connect to the proxy server
    if (connect(client_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Failed to connect to the proxy server");
        return 1;
    }

    // Send request to the proxy server
    char requestBuffer[MAX_BUFFER_SIZE];
    memset(requestBuffer, 0, sizeof(requestBuffer));
    sprintf(requestBuffer, "GET/HTTP/1.1\r\nHost: %s\r\n\r\n", url);

    if (send(client_socket, requestBuffer, strlen(requestBuffer), 0) == -1) {
        perror("Failed to send request to the proxy server");
        return 1;
    }

    // Receive and print response from the proxy server
    char responseBuffer[MAX_BUFFER_SIZE];
    memset(responseBuffer, 0, sizeof(responseBuffer));

    while (1) {
        int bytesRead = recv(client_socket, responseBuffer, sizeof(responseBuffer) - 1, 0);
        if (bytesRead == -1) {
            perror("Failed to recieve response from the proxy server");
            return 1;
        } else if (bytesRead == 0) {
            break; // No more data to receive
        } else {
            printf("%s", responseBuffer);
            memset(responseBuffer, 0, sizeof(responseBuffer));
        }
    }


    close(client_socket);

    return 0;
    }
}
