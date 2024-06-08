#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define PORT 12345
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    struct sockaddr_in address;
    socklen_t addr_len;  // Changed from int to socklen_t
} connection_t;

typedef struct {
    char board[8][8];
    int current_player;
} game_state_t;

game_state_t game_state;
int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t game_lock;

void initialize_board() {
    memset(game_state.board, ' ', sizeof(game_state.board));
    // Initial placement of pieces
    for (int i = 0; i < 3; i++) {
        for (int j = (i % 2 == 0 ? 1 : 0); j < 8; j += 2) {
            game_state.board[i][j] = 'b'; // Black pieces
            game_state.board[7 - i][7 - j] = 'w'; // White pieces
        }
    }
}

void send_to_client(int client_socket, const char *message) {
    send(client_socket, message, strlen(message), 0);
}

void broadcast(const char *message) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        send_to_client(clients[i], message);
    }
}


int main() {
    int server_fd, client1_fd, client2_fd;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    int addrlen = sizeof(server_addr);
    connection_t *connection;
    pthread_t thread;
    char buffer[BUFFER_SIZE];  // Declare buffer here
    int game_code = 1234;  // Example game code initialization

    // Initialize game state
    initialize_board();
    srand(time(NULL));
    game_state.current_player = rand() % 2;

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Bind socket
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);


    if ((client1_fd = accept(server_fd, (struct sockaddr *)&client1_addr, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client 1 connected\n");

    // Accept second client connection
    if ((client2_fd = accept(server_fd, (struct sockaddr *)&client2_addr, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Client 2 connected\n");

    

    close(server_fd);
    pthread_mutex_destroy(&game_lock);
    return 0;
}
