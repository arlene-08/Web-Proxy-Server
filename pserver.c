#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024
#define CACHE_SIZE 5
#define LIST_FILE "list.txt"

typedef struct {
    char url[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];
} CachedWebsite; 

CachedWebsite cache[CACHE_SIZE];

void handle_client(int client_socket) {
    char request[MAX_BUFFER_SIZE];
    char response[MAX_BUFFER_SIZE];
    char target_ip[MAX_BUFFER_SIZE];
    int target_port = 80;

    // Receive client request
    if (recv(client_socket, request, sizeof(request), 0) < 0) {
        perror("Failed to receive client request");
        return;
    }

    int in_cache = 0;
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (strcmp(cache[i].url, target_ip) == 0) {
            // Website found in cache, serve the cached response
            printf("Serving website '%s' from cache...\n", target_ip);
            if (send(client_socket, cache[i].response, strlen(cache[i].response), 0) < 0) {
                perror("Failed to send response to client");
            }
            in_cache = 1;
            break;
        }
    }

    if (!in_cache) {
        // Create a connection to the target server
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(target_port);

        // Check if the HTTP response is 200
        char *http_version = strtok(response, " ");
        char *status_code = strtok(NULL, " ");
        if (http_version != NULL && status_code != NULL && strcmp(status_code, "200") == 0) {
            // Store the response in cache
            CachedWebsite new_website;
            strcpy(new_website.url, target_ip);
            strcpy(new_website.response, response);
            memcpy(&cache[0], &new_website, sizeof(CachedWebsite));

            // Write the URL to the list file
            FILE *list_file = fopen(LIST_FILE, "a");
            if (list_file == NULL) {
                perror("Failed to open list file");
                return;
            }
            fprintf(list_file, "%s\n", target_ip);
            fclose(list_file);
        }
    }
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port_number>\n", argv[0]);
        return 1;
    }


    int port = atoi(argv[1]);

    // Create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Failed to create socket");
        return 1;
    }

    // Bind socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Failed to bind");
        return 1;
    }

    if (listen(server_socket, 5) == -1) {
        perror("Failed to connect");
        return 1;
    }

    printf("Proxy server is running on port %d\n", port);

    while (1) {
        // Accept client connection
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int client_socket = accept(server_socket, (struct sockaddr *)&clientAddr, &addrLen);
        if (client_socket == -1) {
            perror("Failed to accept client connection");
            return 1;
        }

        // Handle client request
        char requestBuffer[MAX_BUFFER_SIZE];
        memset(requestBuffer, 0, sizeof(requestBuffer));

        if (recv(client_socket, requestBuffer, sizeof(requestBuffer), 0) == -1) {
            perror("Failed to recieve client request");
            return 1;
        }

        // Send response back to client
        char responseBuffer[MAX_BUFFER_SIZE];
        memset(responseBuffer, 0, sizeof(responseBuffer));
        
        if (send(client_socket, responseBuffer, strlen(responseBuffer), 0) == -1) {
            perror("Failed to send response to client");
            return 1;
        }

        close(client_socket);
    }

    close(server_socket);

    return 0;
}