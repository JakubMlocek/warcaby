#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "game.h"

#define PORT 12345
#define MAX_CLIENTS 2
#define BOARD_SIZE 8 



int main() {
    int server_fd, client1_fd, client2_fd;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    int addrlen = sizeof(server_addr);

    char board[BOARD_SIZE][BOARD_SIZE];

    initialize_board(board);
    print_board(board);

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    initialize_board(board);
    
    char *buffer = malloc(BUFFER_SIZE);

    memset(buffer, 0, BUFFER_SIZE);

    //waiting for clients
    if ((client1_fd = accept(server_fd, (struct sockaddr *)&client1_addr, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Client 1 connected\n");
    if ((client2_fd = accept(server_fd, (struct sockaddr *)&client2_addr, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Client 2 connected\n");

    // Losowanie ktory z graczy gra X a ktory O
    srand(time(NULL));
    int rand_num = rand() % 2;
    char player1 = (rand_num == 0) ? 'X' : 'O';
    char player2 = (player1 == 'X') ? 'O' : 'X';

    //Przeysłamy powyzsze informacje do klientow
    send(client1_fd, &player1, sizeof(player1), 0);
    send(client2_fd, &player2, sizeof(player2), 0);

    printf("Client 1 is: %c\n", player1);
    printf("Client 2 is: %c\n", player2);

    // Main game loop
    while(1){
        // wyslanie planszy do pierwszego klienta
        buffer = get_buffer_from_board(board);
        //printf("%s\n",buffer);
        send(client1_fd, buffer, BOARD_SIZE * BOARD_SIZE, 0);

        // odbior planszy od pierwszego klienta
        if (recv(client1_fd, buffer, BUFFER_SIZE, 0) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        set_board_to_buffer(board, buffer);
        print_board(board);

        // wyslanie planszy do drugiego klienta
        buffer = get_buffer_from_board(board);
        send(client2_fd, buffer, BUFFER_SIZE, 0);

        // odbior planszy od drugiego klienta
        if (recv(client2_fd, buffer, BUFFER_SIZE, 0) < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }
        set_board_to_buffer(board, buffer);
        print_board(board);
        sleep(1);
    }


    close(server_fd);
    return 0;
}
