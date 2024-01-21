#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void send_file(int socket, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    char buffer[BUFFER_SIZE];
    int bytes_read;
    // send bytes 
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(socket, buffer, bytes_read, 0) != bytes_read) {
            perror("Failed to send file");
            break;
        }
    }

    fclose(file);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) { 
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_UNIX; // AF_UNIX as the client/server comm. is on same machine
    address.sin_addr.s_addr = INADDR_ANY; // Bind to local IP add
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    // int setsockopt(int server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,  &opt, sizeof(opt));
    // Bind the socket to the specified IP address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept a new client connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Connected\n");

    // Send the image file to the client
    send_file(new_socket, "xyz.png");
    printf("Image file sent to client\n");

    return 0;
}
