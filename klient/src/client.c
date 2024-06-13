#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "game.h"

#define SERVER_IP "127.0.0.1"
#define PORT 12345
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
    //Wybor gracza
    char player_char;

    do {
        printf("Wybierz kim grasz (O lub X): ");
        scanf(" %c", &player_char);
        if (player_char != 'O' && player_char != 'X') {
            printf("Zły znak. Wybierz 'O' or 'X'.\n");
        }
    } while (player_char != 'O' && player_char != 'X');

    printf("Jestes graczem: %c\n", player_char);


    // Main game loop
    char *buffer = malloc(BUFFER_SIZE);

    while(1){
        // odbieranie planszy z serwera:
        if (recv(sockfd, buffer, BUFFER_SIZE, 0) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        //printf("%s\n",buffer);
        set_board_to_buffer(board, buffer);
        print_board(board);
        printf("Twoja kolej! Podaj ruch w formacie 'x1 y1 x2 y2' (koordynaty od 1 do 8): ");
        make_move(board, player_char);

        buffer = get_buffer_from_board(board);   
        // Wysylanie buforu z planszy do serwera
        if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
    }
    close(sockfd);
    return 0;
}
