#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 8
#define BUFFER_SIZE 1024


void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if ((i + j) % 2 == 0)
                board[i][j] = ' '; // Puste pole
            else {
                if (i < 3)
                    board[i][j] = 'X'; // Pionki playera 1
                else if (i > BOARD_SIZE - 4)
                    board[i][j] = 'O'; // Pionki playera 2
                else
                    board[i][j] = ' '; // Puste pole
            }
        }
    }
}


void print_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    // Wyświetlenie nagłówka z etykietami kolumn
    printf("   ");
    for (int k = 0; k < BOARD_SIZE; k++) {
        printf("  y%d ", k + 1);
    }
    printf("\n");

    // Wyświetlenie linii oddzielającej nagłówek od planszy
    printf("   +");
    for (int k = 0; k < BOARD_SIZE; k++) {
        printf("----+");
    }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        // Wyświetlenie etykiety wiersza na początku każdego wiersza
        printf("x%d |", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf(" %c  |", board[i][j]);
        }
        printf("\n");
        // Wyświetlenie linii oddzielającej wiersze
        printf("   +");
        for (int k = 0; k < BOARD_SIZE; k++) {
            printf("----+");
        }
        printf("\n");
    }
}

void make_move(char board[BOARD_SIZE][BOARD_SIZE], int x1, int y1, int x2, int y2, char player) {
    x1-=1;
    y1-=1;
    x2-=1;
    y2-=1;

    if (board[x1][y1] != player) {
        printf("To nie jest twoj pionek!\n");
        return;
    }

    if (board[x2][y2] != ' ') {
        printf("Ruch na zajęte pole!\n");
        return;
    }

    if ((player == 'X' && x2 <= x1) || (player == 'O' && x2 >= x1)) {
        printf("Nieprawidłowy kierunek ruchu!\n");
        return;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    if (dx == 1 && dy == 1) {
        board[x2][y2] = player;
        board[x1][y1] = ' ';
        player = (player == 'X') ? 'O' : 'X'; // Zmiana playera
    } else if (dx == 2 && dy == 2 && board[(x1 + x2) / 2][(y1 + y2) / 2] != ' ' && board[(x1 + x2) / 2][(y1 + y2) / 2] != player) {
        board[x2][y2] = player;
        board[x1][y1] = ' ';
        board[(x1 + x2) / 2][(y1 + y2) / 2] = ' '; // Usunięcie piona przeciwnika
    } else {
        printf("Nieprawidłowy ruch!\n");
    }
}

char* get_buffer_from_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    memset(buffer, 0, BUFFER_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            buffer[i * BOARD_SIZE + j] = board[i][j];
        }
    }
    return buffer;
}

void set_board_to_buffer(char board[BOARD_SIZE][BOARD_SIZE], char *buffer) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = buffer[i * BOARD_SIZE + j];
        }
    }
}