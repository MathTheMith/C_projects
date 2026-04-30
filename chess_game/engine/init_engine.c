#include "engine.h"
#include "../chess_game.h"

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

    *bb ^= (from_bit | to_bit);
    if (captured != EMPTY) {
        uint64_t *cbb = get_bitboard(game, captured);
        if (cbb) *cbb ^= to_bit;
    }
}

void undo_move(t_game *game, int piece, int from_x, int from_y, int to_x, int to_y, int captured)
{
    uint64_t *bb = get_bitboard(game, piece);
    if (!bb) return;

    uint64_t from_bit = 1ULL << (from_y * 8 + from_x);
    uint64_t to_bit   = 1ULL << (to_y   * 8 + to_x);

    *bb ^= (from_bit | to_bit);
    if (captured != EMPTY) {
        uint64_t *cbb = get_bitboard(game, captured);
        if (cbb) *cbb ^= to_bit;
    }
}
