#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <time.h>
#include <netdb.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "game.h"

#define PORT 12345
#define MAX_CLIENTS 2
#define BOARD_SIZE 8
#define MULTICAST_GROUP "239.0.0.1" 

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

void error(const char *msg) {
    log_message(msg);
    exit(EXIT_FAILURE);
}

void start_daemon() {
    pid_t pid;
    pid = fork();
    
    if (pid < 0) {
        syslog(LOG_ERR, "Fork failed");
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    
    umask(0);
    openlog("server_daemon", LOG_PID, LOG_DAEMON);

    if (setsid() < 0) {
        syslog(LOG_ERR, "Could not create process group leader.");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Could not change working directory to root.");
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void hostname_to_ip(const char *hostname, char *ip) {
    struct addrinfo hints, *res;
    struct sockaddr_in *sockaddr_ipv4;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // For IPv4
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    sockaddr_ipv4 = (struct sockaddr_in *) res->ai_addr;
    strcpy(ip, inet_ntoa(sockaddr_ipv4->sin_addr));

    freeaddrinfo(res);
}


void play_game(int client1_fd, int client2_fd){
    char board[BOARD_SIZE][BOARD_SIZE];
    initialize_board(board);
    print_board(board);
    char *buffer = malloc(BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);

    while(1){
        // wyslanie planszy do pierwszego klienta
        buffer = get_buffer_from_board(board);
        //printf("%s\n",buffer);
        sendto(client1_fd, buffer, BOARD_SIZE * BOARD_SIZE, 0, NULL, 0);

        // odbior planszy od pierwszego klienta
        if (recvfrom(client1_fd, buffer, BUFFER_SIZE, 0, NULL, 0) < 0) {
            error("recv failed");
            exit(EXIT_FAILURE);
        }
        set_board_to_buffer(board, buffer);
        print_board(board);

        // wyslanie planszy do drugiego klienta
        buffer = get_buffer_from_board(board);
        sendto(client2_fd, buffer, BUFFER_SIZE, 0, NULL, 0);

        // odbior planszy od drugiego klienta
        if (recvfrom(client2_fd, buffer, BUFFER_SIZE, 0, NULL, 0) < 0) {
            error("recv failed");
            exit(EXIT_FAILURE);
        }
        set_board_to_buffer(board, buffer);
        print_board(board);
        sleep(1);
    }
}

void choose_players_pawns(int client1_fd, int client2_fd){
    // Losowanie ktory z graczy gra X a ktory O
    srand(time(NULL));
    int rand_num = rand() % 2;
    char player1 = (rand_num == 0) ? 'X' : 'O';
    char player2 = (player1 == 'X') ? 'O' : 'X';

    //Przeysłamy powyzsze informacje do klientow
    sendto(client1_fd, &player1, sizeof(player1), 0, NULL, 0);
    sendto(client2_fd, &player2, sizeof(player2), 0, NULL, 0);
    
    // char client_1[] = "Client 1 is:";
    // char client_2[] = "Client 2 is:";
    // strcat(client_1, player1);
    // strcat(client_2, player2);

    // log_message(client_1);
    // log_message(client_2);
}

int main() {
    int server_fd, client1_fd, client2_fd;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    int addrlen = sizeof(server_addr);
    char SERVER_IP[INET_ADDRSTRLEN];
    struct ip_mreq multicast_request;
    int multicastTTL = 1;

    log_file = fopen("/var/log/game_server.log", "a+");
    if (!log_file) {
        syslog(LOG_ERR, "Could not open log file.");
        exit(EXIT_FAILURE);
    }

    //Utworzenie nasluchwiania na serwerze
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        error("Socket creation failed");
        exit(EXIT_FAILURE);
    }


    int optval = 1;
    setsockopt(server_fd, IPPROTO_IP, IP_MULTICAST_TTL, &multicast_request, sizeof(multicast_request));

    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);
    
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    

    // char server_str[] = "Server listening on port "; 
    // char port_str[] = PORT;
    // strcat(server_str, PORT);
    // log_message(server_str);

    //Przyjecie polaczen od 2 klientow

    choose_players_pawns(client1_fd, client2_fd);

    play_game(client1_fd, client2_fd);

    close(server_fd);
    return 0;
}
