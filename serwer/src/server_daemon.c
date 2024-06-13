#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include "game.h"

#define PORT 12345
#define MAX_CLIENTS 2
#define BOARD_SIZE 8 
#define BUFFER_SIZE 1024
#define HOSTNAME "localhost"
#define SERVER_IP "127.0.0.1..." 

FILE *log_file;

void log_message(const char *msg) {
    char buffer[256];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(buffer, sizeof(buffer) - 1, "%b %d %H:%M:%S", t);

    char log_msg[512];
    snprintf(log_msg, sizeof(log_msg), "%s %s", buffer, msg);

    syslog(LOG_INFO, "%s", log_msg);
    if (log_file) {
        fprintf(log_file, "%s\n", log_msg);
        fflush(log_file);
    }
}

void start_daemon() {
    pid_t pid;

    // Fork the process
    pid = fork();

    if (pid < 0) {
        syslog(LOG_ERR, "Fork failed");
        exit(EXIT_FAILURE);
    }

    // If we got a good PID, then we can exit the parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Change the file mode mask
    umask(0);

    // Open logs
    openlog("server_daemon", LOG_PID, LOG_DAEMON);

    // Create a new SID for the child process
    if (setsid() < 0) {
        syslog(LOG_ERR, "Could not create process group leader.");
        exit(EXIT_FAILURE);
    }

    // Change the current working directory to root
    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Could not change working directory to root.");
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void error(const char *msg) {
    log_message(msg);
    exit(EXIT_FAILURE);
}


// void hostname_to_ip(const char *hostname, char *ip) {
//     struct addrinfo hints, *res;
//     struct sockaddr_in *sockaddr_ipv4;

//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_INET; // For IPv4
//     hints.ai_socktype = SOCK_STREAM;

//     int status = getaddrinfo(hostname, NULL, &hints, &res);
//     if (status != 0) {
//         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
//         exit(EXIT_FAILURE);
//     }

//     sockaddr_ipv4 = (struct sockaddr_in *) res->ai_addr;
//     strcpy(ip, inet_ntoa(sockaddr_ipv4->sin_addr));

//     freeaddrinfo(res);
// }

int main() {
    int server_fd, client1_fd, client2_fd;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    socklen_t addrlen = sizeof(server_addr);

    char board[BOARD_SIZE][BOARD_SIZE];
    // char SERVER_IP[INET_ADDRSTRLEN];

    // Initialize logging
    log_file = fopen("/var/log/game_server.log", "a+");
    if (!log_file) {
        syslog(LOG_ERR, "Could not open log file.");
        exit(EXIT_FAILURE);
    }

    log_message("Initializing board.");
    initialize_board(board);
    print_board(board);

    // Create server socket
    log_message("Creating server socket.");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        error("Socket creation failed");
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // hostname_to_ip(HOSTNAME, SERVER_IP);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    log_message("Binding server socket.");
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Bind failed");
    }

    log_message("Listening for connections.");
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        error("Listen failed");
    }

    printf("Server listening on port %d\n", PORT);
    log_message("Server listening on port.");

    initialize_board(board);

    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        error("Memory allocation failed");
    }

    memset(buffer, 0, BUFFER_SIZE);

    // Waiting for clients
    if ((client1_fd = accept(server_fd, (struct sockaddr *)&client1_addr, &addrlen)) < 0) {
        error("Accept failed for client 1");
    }
    printf("Client 1 connected\n");
    log_message("Client 1 connected.");

    if ((client2_fd = accept(server_fd, (struct sockaddr *)&client2_addr, &addrlen)) < 0) {
        error("Accept failed for client 2");
    }
    printf("Client 2 connected\n");
    log_message("Client 2 connected.");

    // Randomize player symbols
    srand(time(NULL));
    int rand_num = rand() % 2;
    char player1 = (rand_num == 0) ? 'X' : 'O';
    char player2 = (player1 == 'X') ? 'O' : 'X';

    // Send player symbols to clients
    send(client1_fd, &player1, sizeof(player1), 0);
    send(client2_fd, &player2, sizeof(player2), 0);

    printf("Client 1 is: %c\n", player1);
    printf("Client 2 is: %c\n", player2);
    log_message("Players assigned symbols.");

    // Main game loop
    while(1) {
        // Send board to first client
        buffer = get_buffer_from_board(board);
        send(client1_fd, buffer, BOARD_SIZE * BOARD_SIZE, 0);

        // Receive board from first client
        if (recv(client1_fd, buffer, BUFFER_SIZE, 0) < 0) {
            error("Receive failed from client 1");
        }
        set_board_to_buffer(board, buffer);
        print_board(board);

        // Send board to second client
        buffer = get_buffer_from_board(board);
        send(client2_fd, buffer, BUFFER_SIZE, 0);

        // Receive board from second client
        if (recv(client2_fd, buffer, BUFFER_SIZE, 0) < 0) {
            error("Receive failed from client 2");
        }
        set_board_to_buffer(board, buffer);
        print_board(board);
        sleep(1);
    }

    close(server_fd);
    fclose(log_file);
    return 0;
}
