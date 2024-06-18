#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "game.h"

#define PORT 12345
#define BOARD_SIZE 8
#define BUFFER_SIZE 1024

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

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char SERVER_IP[INET_ADDRSTRLEN];

    // Create client socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    hostname_to_ip("server-host", SERVER_IP);
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

    // Receive information from the server about which player you are
    char player_char;
    if (recv(sockfd, &player_char, sizeof(player_char), 0) < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }

    printf("You are player: %c\n", player_char);

    // Main game loop
    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Receive board data from the server
        ssize_t bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        }

        // Decode received TLV data
        char *decoded_data = tlv_decode((unsigned char *)buffer, bytes_received);
        if (decoded_data == NULL) {
            fprintf(stderr, "Failed to decode TLV data\n");
            exit(EXIT_FAILURE);
        }

        // Update local board from received data
        set_board_to_buffer(board, decoded_data);
        print_board(board);

        // Make a move on the local board
        make_move(board, player_char);
        print_board(board);

        // Encode board data to send back to the server
        size_t tlv_size;
        char *board_buffer = get_buffer_from_board(board);
        unsigned char *encoded_data = tlv_encode(board_buffer, strlen(board_buffer), &tlv_size);
        free(board_buffer);
        if (encoded_data == NULL) {
            fprintf(stderr, "Failed to encode TLV data\n");
            exit(EXIT_FAILURE);
        }

        // Send updated board data to the server
        if (send(sockfd, encoded_data, tlv_size, 0) < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }

        // Clean up allocated memory
        free(decoded_data);
        free(encoded_data);
    }

    free(buffer);
    close(sockfd);
    return 0;
}
