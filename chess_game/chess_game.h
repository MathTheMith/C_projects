#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include "raylib.h"
#include <stdint.h>
#include <stdbool.h>

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
    uint64_t pawns;
    uint64_t rooks;
    uint64_t knight;
    uint64_t bishop;
    uint64_t queen;
    uint64_t king;
} t_bitboards;

typedef struct {
    t_bitboards wp;
    t_bitboards bp;
    int current_piece;
    int current_turn;
    int is_piece;
    int robot_color;
    int old_x;
    int old_y;
    int ep_square;
    uint8_t castling;
} t_game;

typedef struct {
    int from_x, from_y;
    int to_x, to_y;
    int captured;
} t_move;

static inline uint64_t get_white_bb(t_game *game) {
    return game->wp.pawns | game->wp.rooks | game->wp.knight
         | game->wp.bishop | game->wp.queen | game->wp.king;
}

static inline uint64_t get_black_bb(t_game *game) {
    return game->bp.pawns | game->bp.rooks | game->bp.knight
         | game->bp.bishop | game->bp.queen | game->bp.king;
}

static inline uint64_t get_occupied_bb(t_game *game) {
    return get_white_bb(game) | get_black_bb(game);
}

int       get_piece(t_game *game, int sq);
bool      is_piece_on_square(t_game *game, int x, int y);
bool      can_capture(t_game *game, int from_sq, int to_sq);
void      set_board();
void      show_possibles_moves(t_game *game, int x, int y);
void      show_rook_moves(t_game *game, int x, int y);
void      show_bishop_moves(t_game *game, int x, int y);
void      show_queen_moves(t_game *game, int x, int y);
void      show_king_moves(t_game *game, int x, int y);
void      show_knight_moves(t_game *game, int x, int y);
bool      is_valid_move(t_game *game, int x, int y);
void      show_wpawns_moves(t_game *game, int x, int y);
void      show_bpawns_moves(t_game *game, int x, int y);
void      init_board();
bool      is_in_check(t_game *game);
bool      has_legal_moves(t_game *game);
bool      is_valid_destination(t_game *game, int old_x, int old_y, int new_x, int new_y);
bool      is_valid_rook_move(t_game *game, int old_x, int old_y, int new_x, int new_y);
bool      is_valid_bishop_move(t_game *game, int old_x, int old_y, int new_x, int new_y);
bool      is_valid_knight_move(t_game *game, int old_x, int old_y, int new_x, int new_y);
bool      is_valid_queen_move(t_game *game, int old_x, int old_y, int new_x, int new_y);
bool      is_valid_king_move(t_game *game, int old_x, int old_y, int new_x, int new_y);
bool      is_valid_wpawn_move(t_game *game, int old_x, int old_y, int new_x, int new_y);
bool      is_valid_bpawn_move(t_game *game, int old_x, int old_y, int new_x, int new_y);
Texture2D get_texture(int piece);
void      UnloadTextures();
bool      game_over(t_game *game);
void      move_pieces(t_game *game, int x, int y);
void      undo_move(t_game *game, int piece, int from_x, int from_y, int to_x, int to_y, int captured);
int       evaluate(t_game *game);
t_move    generate_moves(t_game *game);

#endif
