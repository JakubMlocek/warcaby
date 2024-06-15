#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "game.h"

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BOARD_SIZE 8


void play_game(int sockfd, char player_char){
    char board[BOARD_SIZE][BOARD_SIZE];
    initialize_board(board);
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
        make_move(board, player_char);
        print_board(board);
        buffer = get_buffer_from_board(board);   
        // Wysylanie buforu z planszy do serwera
        if (send(sockfd, buffer, BUFFER_SIZE, 0) < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

	int optval = 1;	
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);
    
    //Odbieramy informacje od serwera, ktorym graczem jestesmy.
    char player_char;
    if (recv(sockfd, &player_char, BUFFER_SIZE, 0) < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    printf("Jestes graczem: %c\n", player_char);

    //rozpoczynamy gre
    play_game(sockfd, player_char);

    close(sockfd);
    return 0;
}
