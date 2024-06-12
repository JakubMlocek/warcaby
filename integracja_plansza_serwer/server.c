#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "warcaby.h"

#define PORT 12345
#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024
#define ROZMIAR_PLANSZY 8 

typedef struct {
    int socket;
    struct sockaddr_in address;
    socklen_t addr_len;  // Changed from int to socklen_t
} connection_t;

int clients[MAX_CLIENTS];
int client_count = 0;

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

    // Inicjalizacja zmiennych do gry
    char plansza[ROZMIAR_PLANSZY][ROZMIAR_PLANSZY];
    srand(time(NULL));
    char obecny_gracz = (rand() % 2 == 0) ? 'X' : 'O'; // Losowanie początkowego gracza
    int x1, y1, x2, y2;

    inicjalizujPlansze(plansza);
    wyswietlPlansze(plansza);

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



    //for (int i = 0; i < 5; i++) {
    //    printf("Ruch gracza %c. Podaj ruch w formacie 'x1 y1 x2 y2' (koordynaty od 1 do 8): ", gracz);
    //   scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
    //    wykonajRuch(plansza, x1 - 1, y1 - 1, x2 - 1, y2 - 1, &gracz);
    //    wyswietlPlansze(plansza);
    //}

    close(server_fd);
    pthread_mutex_destroy(&game_lock);
    return 0;
}
