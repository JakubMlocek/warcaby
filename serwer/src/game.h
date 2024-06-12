
#ifndef GAME_H
#define GAME_H

#define BOARD_SIZE 8
#define BOARD_SIZE 8
#define BUFFER_SIZE 1024


void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]);

void print_board(char board[BOARD_SIZE][BOARD_SIZE]);

void make_move(char board[BOARD_SIZE][BOARD_SIZE], int x1, int y1, int x2, int y2, char gracz);

char* get_buffer_from_board(char board[BOARD_SIZE][BOARD_SIZE]);

void set_board_to_buffer(char board[BOARD_SIZE][BOARD_SIZE], char *buffer);

#endif // GAME_H