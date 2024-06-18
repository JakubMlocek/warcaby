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
#include <errno.h>
#include "game.h"

// #define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BOARD_SIZE 8
#define MULTICAST_GROUP "239.0.0.1"

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

void play_game(int sockfd, char player_char){
    char board[BOARD_SIZE][BOARD_SIZE];
    initialize_board(board);
    char *buffer = malloc(BUFFER_SIZE);


    while(1){
        // odbieranie planszy z serwera:
        if (recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, 0) < 0) {
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
        if (sendto(sockfd, buffer, BUFFER_SIZE, 0, NULL, 0) < 0) {
            perror("send failed");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct ip_mreq multicast_request;
    char SERVER_IP[INET_ADDRSTRLEN];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    

    setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_request, sizeof(multicast_request));


    multicast_request.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_request.imr_interface.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    
    
    //Odbieramy informacje od serwera, ktorym graczem jestesmy.
    char player_char;
    if (recvfrom(sockfd, &player_char, BUFFER_SIZE, 0, NULL, 0) < 0) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    printf("Jestes graczem: %c\n", player_char);

    //rozpoczynamy gre
    play_game(sockfd, player_char);

    close(sockfd);
    return 0;
}
