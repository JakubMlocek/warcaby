#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 8
#define BUFFER_SIZE 1048


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
    // WyÅ›wietlenie naglowka z etykietami kolumn
    printf("   ");
    for (int k = 0; k < BOARD_SIZE; k++) {
        printf("  y%d ", k + 1);
    }
    printf("\n");

    // WyÅ›wietlenie linii oddzielajÄ…cej naglowek od planszy
    printf("   +");
    for (int k = 0; k < BOARD_SIZE; k++) {
        printf("----+");
    }
    printf("\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        // WyÅ›wietlenie etykiety wiersza na poczÄ…tku kazdego wiersza
        printf("x%d |", i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf(" %c  |", board[i][j]);
        }
        printf("\n");
        // WyÅ›wietlenie linii oddzielajÄ…cej wiersze
        printf("   +");
        for (int k = 0; k < BOARD_SIZE; k++) {
            printf("----+");
        }
        printf("\n");
    }
}

void make_move(char board[BOARD_SIZE][BOARD_SIZE], char player) {
    int correct_move = 0;
    int x1, y1, x2, y2;

    while(!correct_move){
        printf("Twoja kolej! Podaj ruch w formacie 'x1 y1 x2 y2' (koordynaty od 1 do 8): ");

        scanf("%d %d %d %d", &x1, &y1, &x2, &y2);

        if (x1 > BOARD_SIZE || x2 > BOARD_SIZE || y1 > BOARD_SIZE || y2 > BOARD_SIZE){
            printf("Nie ma takiego pola!\n");
            continue;
        }

        if (x1 < 1  || x2 < 1 || y1 < 1 || y2 < 1){
            printf("Nie ma takiego pola!\n");
            continue;
        }

        x1-=1;
        y1-=1;
        x2-=1;
        y2-=1;

        char piece = board[x1][y1];

        if (piece != player && piece != 'D' && piece != 'Q') {
            printf("To nie jest twoj pionek!\n");
            continue;
        }

        if (board[x2][y2] != ' ') {
            printf("Ruch na zajÄ™te pole!\n");
            continue;
        }

        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);

        //Rozwazamy ruch zwyklego piona
        if (piece == 'X' || piece == 'O') {
            if ((player == 'X' && x2 <= x1) || (player == 'O' && x2 >= x1)) {
                printf("Nieprawidlowy kierunek ruchu!\n");
                continue;;
            }

            if (dx == 1 && dy == 1) {
                //zwykly ruch
                board[x2][y2] = player;
                board[x1][y1] = ' ';
                // Sprawdzenie czy pionek osiÄ…gnÄ…l ostatni rzÄ…d i zamiana go na damkÄ™
                if (player == 'X' && x2 == BOARD_SIZE - 1) {
                    board[x2][y2] = 'D';
                } else if (player == 'O' && x2 == 0) {
                    board[x2][y2] = 'Q';
                }
                correct_move = 1;
            } else if (dx == 2 && dy == 2) {
                //zbijanie piona przeciwnika
                int mid_x = (x1 + x2) / 2;
                int mid_y = (y1 + y2) / 2;
                char opponent = (player == 'X') ? 'O' : 'X';
                if (board[mid_x][mid_y] == opponent) {
                    board[x2][y2] = player;
                    board[x1][y1] = ' ';
                    board[mid_x][mid_y] = ' ';
                    // Sprawdzenie czy pionek osiÄ…gnÄ…l ostatni rzÄ…d i zamiana go na damkÄ™
                    if (player == 'X' && x2 == BOARD_SIZE - 1) {
                        board[x2][y2] = 'D';
                    } else if (player == 'O' && x2 == 0) {
                        board[x2][y2] = 'Q';
                    }
                    correct_move = 1;
                } else {
                    printf("Nieprawidlowy ruch! Brak pionka przeciwnika do zbicia.\n");
                    continue;
                }
            } else {
                printf("Nieprawidlowy ruch!\n");
                continue;
            }
        //Rozwazmy ruch damki
        } else if (piece == 'D' || piece == 'Q') {
            // Ruchy damki
            if (dx == dy) {
                int direction_x = (x2 - x1) / dx; //kierunek ruchu po x
                int direction_y = (y2 - y1) / dy; //kierunek ruchu po y
                int opponent_pieces = 0;
                int own_pieces = 0;
                int mid_x, mid_y;


                //zliczanie czy po drodze puste pola lub co najwyzej jeden pion przeciwnika
                for (int i = 1; i < dx; i++) {
                    int check_x = x1 + i * direction_x;
                    int check_y = y1 + i * direction_y;
                    if (board[check_x][check_y] != ' ') {
                        if (board[check_x][check_y] == (piece == 'D' ? 'O' : 'X')) {
                            opponent_pieces++;
                            mid_x = check_x;
                            mid_y = check_y;
                        } else { 
                            own_pieces++;
                        }
                    }
                }
                if (own_pieces > 0){
                    printf("Nieprawidlowy ruch! Wlasny pion na drodze!.\n");
                }
                else if (opponent_pieces <= 1) {  
                    board[x2][y2] = piece;
                    board[x1][y1] = ' ';
                    if (opponent_pieces == 1) {
                        board[mid_x][mid_y] = ' ';
                    }
                    correct_move = 1;  // Ruch jest poprawny
                } else {
                    printf("Nieprawidlowy ruch! Przeszkoda na drodze lub za duzo pionkow przeciwnika do zbicia.\n");
                    continue;
                }
            }
        } else {
            printf("Nieprawidlowy ruch!\n");
            continue;
        }
        
        printf("Trwa ruch przeciwnika.\n");

    }
    return;
}

int check_pieces_left(char board[BOARD_SIZE][BOARD_SIZE], char player) {
    int count = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == player) {
                count++;
            }
        }
    }
    return count;
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

void set_board_to_buffer(char board[BOARD_SIZE][BOARD_SIZE], char* buffer) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = buffer[i * BOARD_SIZE + j];
        }
    }
}