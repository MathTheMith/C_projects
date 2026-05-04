#include "engine.h"
#include "../chess_game.h"
#include <stdlib.h>

uint64_t zobrist_table[12][64];
uint64_t zobrist_black_turn;

void set_pieces(t_bitboards *wp, t_bitboards *bp)
{
    wp->pawns  = 0x00FF000000000000ULL;
    wp->rooks  = 0x8100000000000000ULL;
    wp->knight = 0x4200000000000000ULL;
    wp->bishop = 0x2400000000000000ULL;
    wp->queen  = 0x0800000000000000ULL;
    wp->king   = 0x1000000000000000ULL;
    bp->pawns  = 0x000000000000FF00ULL;
    bp->rooks  = 0x0000000000000081ULL;
    bp->knight = 0x0000000000000042ULL;
    bp->bishop = 0x0000000000000024ULL;
    bp->queen  = 0x0000000000000008ULL;
    bp->king   = 0x0000000000000010ULL;
}

void init_zobrist(void)
{
    srand(0xDEADBEEF);
    for (int p = 0; p < 12; p++)
        for (int sq = 0; sq < 64; sq++)
            zobrist_table[p][sq] = ((uint64_t)rand() << 32) | (uint32_t)rand();
    zobrist_black_turn = ((uint64_t)rand() << 32) | (uint32_t)rand();
}

uint64_t compute_hash(t_game *game)
{
    uint64_t h = 0;
    for (int sq = 0; sq < 64; sq++) {
        int p = get_piece(game, sq);
        if (p != EMPTY)
            h ^= zobrist_table[piece_to_index(p)][sq];
    }
    if (game->current_turn == 1)
        h ^= zobrist_black_turn;
    return h;
}

static uint64_t *get_bitboard(t_game *game, int piece)
{
    if (piece == WP) return &game->wp.pawns;
    if (piece == WR) return &game->wp.rooks;
    if (piece == WN) return &game->wp.knight;
    if (piece == WB) return &game->wp.bishop;
    if (piece == WQ) return &game->wp.queen;
    if (piece == WK) return &game->wp.king;
    if (piece == BP) return &game->bp.pawns;
    if (piece == BR) return &game->bp.rooks;
    if (piece == BN) return &game->bp.knight;
    if (piece == BB) return &game->bp.bishop;
    if (piece == BQ) return &game->bp.queen;
    if (piece == BK) return &game->bp.king;
    return NULL;
}

void move_pieces(t_game *game, int x, int y)
{
    int from_sq  = game->old_y * 8 + game->old_x;
    int to_sq    = y * 8 + x;
    int piece    = get_piece(game, from_sq);
    int captured = get_piece(game, to_sq);

    uint64_t *bb = get_bitboard(game, piece);
    if (!bb) return;

    uint64_t from_bit = 1ULL << from_sq;
    uint64_t to_bit   = 1ULL << to_sq;

    game->hash ^= zobrist_table[piece_to_index(piece)][from_sq];
    if (captured != EMPTY)
        game->hash ^= zobrist_table[piece_to_index(captured)][to_sq];

    if ((piece == WP || piece == BP) && game->old_x != x && captured == EMPTY) {
        uint64_t *cbb = (piece == WP) ? &game->bp.pawns : &game->wp.pawns;
        int ep_sq = game->old_y * 8 + x;
        *cbb &= ~(1ULL << ep_sq);
        game->hash ^= zobrist_table[piece_to_index(piece == WP ? BP : WP)][ep_sq];
    }

    *bb ^= (from_bit | to_bit);
    if (captured != EMPTY) {
        uint64_t *cbb = get_bitboard(game, captured);
        if (cbb) *cbb ^= to_bit;
    }

    if ((piece == WK || piece == BK) && abs(x - game->old_x) == 2) {
        uint64_t *rbb = (piece == WK) ? &game->wp.rooks : &game->bp.rooks;
        int rook = (piece == WK) ? WR : BR;
        if (x > game->old_x) {
            int r_from = game->old_y * 8 + 7, r_to = game->old_y * 8 + 5;
            *rbb ^= (1ULL << r_from) | (1ULL << r_to);
            game->hash ^= zobrist_table[piece_to_index(rook)][r_from];
            game->hash ^= zobrist_table[piece_to_index(rook)][r_to];
        } else {
            int r_from = game->old_y * 8 + 0, r_to = game->old_y * 8 + 3;
            *rbb ^= (1ULL << r_from) | (1ULL << r_to);
            game->hash ^= zobrist_table[piece_to_index(rook)][r_from];
            game->hash ^= zobrist_table[piece_to_index(rook)][r_to];
        }
    }

    game->hash ^= zobrist_table[piece_to_index(piece)][to_sq];
    game->hash ^= zobrist_black_turn;

    if (piece == WP && (game->old_y - y) == 2)
        game->ep_square = (y + 1) * 8 + x;
    else if (piece == BP && (y - game->old_y) == 2)
        game->ep_square = (y - 1) * 8 + x;
    else
        game->ep_square = -1;

    if (piece == WK) game->castling &= ~0x3;
    if (piece == BK) game->castling &= ~0xC;
    if (piece == WR) {
        if (game->old_x == 7 && game->old_y == 7) game->castling &= ~0x1;
        if (game->old_x == 0 && game->old_y == 7) game->castling &= ~0x2;
    }
    if (piece == BR) {
        if (game->old_x == 7 && game->old_y == 0) game->castling &= ~0x4;
        if (game->old_x == 0 && game->old_y == 0) game->castling &= ~0x8;
    }
    if (captured == WR) {
        if (x == 7 && y == 7) game->castling &= ~0x1;
        if (x == 0 && y == 7) game->castling &= ~0x2;
    }
    if (captured == BR) {
        if (x == 7 && y == 0) game->castling &= ~0x4;
        if (x == 0 && y == 0) game->castling &= ~0x8;
    }
}

void undo_move(t_game *game, int piece, int from_x, int from_y, int to_x, int to_y, int captured)
{
    uint64_t *bb = get_bitboard(game, piece);
    if (!bb) return;

    uint64_t from_bit = 1ULL << (from_y * 8 + from_x);
    uint64_t to_bit   = 1ULL << (to_y   * 8 + to_x);

    game->hash ^= zobrist_black_turn;
    game->hash ^= zobrist_table[piece_to_index(piece)][to_y * 8 + to_x];
    game->hash ^= zobrist_table[piece_to_index(piece)][from_y * 8 + from_x];
    if (captured != EMPTY)
        game->hash ^= zobrist_table[piece_to_index(captured)][to_y * 8 + to_x];

    *bb ^= (from_bit | to_bit);
    if (captured != EMPTY) {
        uint64_t *cbb = get_bitboard(game, captured);
        if (cbb) *cbb ^= to_bit;
    }

    if ((piece == WK || piece == BK) && abs(from_x - to_x) == 2) {
        uint64_t *rbb = (piece == WK) ? &game->wp.rooks : &game->bp.rooks;
        int rook = (piece == WK) ? WR : BR;
        if (to_x > from_x) {
            int r_from = from_y * 8 + 5, r_to = from_y * 8 + 7;
            *rbb ^= (1ULL << r_from) | (1ULL << r_to);
            game->hash ^= zobrist_table[piece_to_index(rook)][r_from];
            game->hash ^= zobrist_table[piece_to_index(rook)][r_to];
        } else {
            int r_from = from_y * 8 + 3, r_to = from_y * 8 + 0;
            *rbb ^= (1ULL << r_from) | (1ULL << r_to);
            game->hash ^= zobrist_table[piece_to_index(rook)][r_from];
            game->hash ^= zobrist_table[piece_to_index(rook)][r_to];
        }
    }

    if ((piece == WP || piece == BP) && from_x != to_x && captured == EMPTY) {
        uint64_t *cbb = (piece == WP) ? &game->bp.pawns : &game->wp.pawns;
        int ep_sq = from_y * 8 + to_x;
        *cbb |= 1ULL << ep_sq;
        game->hash ^= zobrist_table[piece_to_index(piece == WP ? BP : WP)][ep_sq];
    }
}
