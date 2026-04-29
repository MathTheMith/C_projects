#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include "raylib.h"

#define BOARD_SIZE 8

#define EMPTY  0
#define WP     1
#define WR     2
#define WN     3
#define WB     4
#define WQ     5
#define WK     6
#define BP    -1
#define BR    -2
#define BN    -3
#define BB    -4
#define BQ    -5
#define BK    -6

extern Texture2D wR, bR, wN, bN, wB, bB, wK, bK, wQ, bQ, wP, bP;

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    int current_piece;
    int current_turn;
    int is_piece;
    int old_x;
    int old_y;
} t_game;

bool is_piece_on_square(int board[BOARD_SIZE][BOARD_SIZE], int x, int y);
bool can_capture(int board[BOARD_SIZE][BOARD_SIZE], int x, int y, int old_x, int old_y);
void set_board();
void show_possibles_moves(int board[BOARD_SIZE][BOARD_SIZE], int x, int y);
void show_rook_moves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void show_bishop_moves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void show_queen_moves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void show_king_moves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void show_knight_moves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
bool is_valid_move(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void show_wpawns_moves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void show_bpawns_moves(int x, int y, int board[BOARD_SIZE][BOARD_SIZE]);
void init_board(int board[BOARD_SIZE][BOARD_SIZE]);
bool is_valid_destination(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
bool is_valid_rook_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
bool is_valid_bishop_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
bool is_valid_knight_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
bool is_valid_queen_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
bool is_valid_king_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
bool is_valid_wpawn_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
bool is_valid_bpawn_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y);
Texture2D get_texture(int piece);
void UnloadTextures();

#endif
