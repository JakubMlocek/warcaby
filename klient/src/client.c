#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "game.h"

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024
#define BOARD_SIZE 8


int main() {
    int sockfd;
    struct sockaddr_in server_addr;

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

    char board[BOARD_SIZE][BOARD_SIZE];
    initialize_board(board);

    // Main game loop
    
    char* buffer = get_buffer_from_board(board);
    // Receive the game board buffer from the server
    if (recv(sockfd, buffer, sizeof(buffer), 0) < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    printf("%s",buffer);

    make_move(board, 6, 1, 5, 2, 'O');
    print_board(board);

    buffer = get_buffer_from_board(board);   
   
    printf("%s",buffer);
    // Send buffer with board after our move to server
    if (send(sockfd, buffer, sizeof(buffer), 0) < 0) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    

    close(sockfd);
    return 0;
}
