#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int sockfd;

void signal_handler(int sig);

int main() {
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    

    // Create client socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

	int optval = 1;	
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Set server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);

    // Main game loop
    
	while (1) {
        signal(SIGINT, signal_handler);
        if ((bytes_read = read(sockfd, buffer, BUFFER_SIZE)) < 0) {
            perror("Reading from socket failed");
            exit(EXIT_FAILURE);
        }
        buffer[bytes_read] = '\0';
        printf("Received message: %s\n", buffer);
	}


    close(sockfd);
    return 0;
}

void signal_handler(int sig) {
    printf("Ctrl+C received. Closing socket and exiting...\n");
    // Close the socket gracefully
    close(sockfd);
    // Exit the program
    exit(0);
}