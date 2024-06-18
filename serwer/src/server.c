#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
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

FILE *log_file;

unsigned char* tlv_encode(const char* data, size_t data_len, size_t* tlv_len) {
    // Calculate total TLV length
    *tlv_len = 2 + data_len;  // 1 byte for type + 1 byte for length + data length

    // Allocate memory for TLV buffer
    unsigned char* tlv_data = (unsigned char*)malloc(*tlv_len);
    if (tlv_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Type (assuming 1 byte, you can change it as per your requirements)
    tlv_data[0] = 0x01;

    // Length (assuming 1 byte, you can change it as per your requirements)
    tlv_data[1] = (unsigned char)data_len;

    // Copy data into TLV buffer
    memcpy(&tlv_data[2], data, data_len);

    return tlv_data;
}

// Function to decode TLV data
char* tlv_decode(const unsigned char* tlv_data, size_t tlv_len) {
    // Validate minimum TLV length
    if (tlv_len < 2) {
        fprintf(stderr, "Invalid TLV data\n");
        return NULL;
    }

    // Type (assuming 1 byte, you can change it as per your requirements)
    unsigned char tlv_type = tlv_data[0];

    // Length (assuming 1 byte, you can change it as per your requirements)
    unsigned char tlv_length = tlv_data[1];

    // Validate total TLV length
    if (tlv_len != 2 + tlv_length) {
        fprintf(stderr, "Invalid TLV length\n");
        return NULL;
    }

    // Allocate memory for data string
    char* data = (char*)malloc(tlv_length + 1);
    if (data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    // Copy value part into data string
    memcpy(data, &tlv_data[2], tlv_length);
    data[tlv_length] = '\0';  // Null-terminate the string

    return data;
}


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
        size_t tlv_size;
        size_t original_len = strlen(buffer);

        buffer = tlv_encode(buffer, original_len, &tlv_size);
        //printf("%s\n",buffer);
        
        send(client1_fd, buffer, BOARD_SIZE * BOARD_SIZE, 0);

        // odbior planszy od pierwszego klienta
        if (recv(client1_fd, buffer, BUFFER_SIZE, 0) < 0) {
            error("recv failed");
            exit(EXIT_FAILURE);
        }

        buffer = tlv_decode(buffer, tlv_size);
        set_board_to_buffer(board, buffer);
        print_board(board);

        original_len = strlen(buffer);

        // wyslanie planszy do drugiego klienta
        buffer = tlv_encode(get_buffer_from_board(board), original_len, &tlv_size);
        send(client2_fd, buffer, BUFFER_SIZE, 0);

        // odbior planszy od drugiego klienta
        if (recv(client2_fd, buffer, BUFFER_SIZE, 0) < 0) {
            error("recv failed");
            exit(EXIT_FAILURE);
        }
        buffer = tlv_decode(buffer, tlv_size);
        set_board_to_buffer(board, buffer);
        print_board(board);
        original_len = strlen(buffer);
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
    send(client1_fd, &player1, sizeof(player1), 0);
    send(client2_fd, &player2, sizeof(player2), 0);
    
}


int main() {
    int server_fd, client1_fd, client2_fd;
    struct sockaddr_in server_addr, client1_addr, client2_addr;
    int addrlen = sizeof(server_addr);
    char SERVER_IP[INET_ADDRSTRLEN];
    size_t tlv_size;

    log_file = fopen("/var/log/game_server.log", "a+");
    if (!log_file) {
        syslog(LOG_ERR, "Could not open log file.");
        exit(EXIT_FAILURE);
    }

    //Utworzenie nasluchwiania na serwerze
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        error("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    hostname_to_ip("server-host", SERVER_IP);   

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        error("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        error("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    char server_str[] = "Server listening on port "; 
    log_message(server_str);

    //Przyjecie polaczen od 2 klientow
    if ((client1_fd = accept(server_fd, (struct sockaddr *)&client1_addr, (socklen_t*)&addrlen)) < 0) {
        error("accept");
        exit(EXIT_FAILURE);
    }
    log_message("Client 1 connected\n");
    if ((client2_fd = accept(server_fd, (struct sockaddr *)&client2_addr, (socklen_t*)&addrlen)) < 0) {
        error("accept");
        exit(EXIT_FAILURE);
    }
    log_message("Client 2 connected\n");

    log_message("Starting game...");

    choose_players_pawns(client1_fd, client2_fd);

    play_game(client1_fd, client2_fd);

    close(server_fd);
    return 0;
}
