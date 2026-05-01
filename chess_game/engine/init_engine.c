#include "engine.h"
#include "../chess_game.h"
#include <stdlib.h>

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

    // En passant: diagonal pawn move to empty square
    if ((piece == WP || piece == BP) && game->old_x != x && captured == EMPTY) {
        uint64_t *cbb = (piece == WP) ? &game->bp.pawns : &game->wp.pawns;
        *cbb &= ~(1ULL << (game->old_y * 8 + x));
    }

    *bb ^= (from_bit | to_bit);
    if (captured != EMPTY) {
        uint64_t *cbb = get_bitboard(game, captured);
        if (cbb) *cbb ^= to_bit;
    }

    // Castling: also move the rook
    if ((piece == WK || piece == BK) && abs(x - game->old_x) == 2) {
        uint64_t *rbb = (piece == WK) ? &game->wp.rooks : &game->bp.rooks;
        if (x > game->old_x)
            *rbb ^= (1ULL << (game->old_y * 8 + 7)) | (1ULL << (game->old_y * 8 + 5));
        else
            *rbb ^= (1ULL << (game->old_y * 8 + 0)) | (1ULL << (game->old_y * 8 + 3));
    }

    // Update ep_square
    if (piece == WP && (game->old_y - y) == 2)
        game->ep_square = (y + 1) * 8 + x;
    else if (piece == BP && (y - game->old_y) == 2)
        game->ep_square = (y - 1) * 8 + x;
    else
        game->ep_square = -1;

    // Update castling rights
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

    *bb ^= (from_bit | to_bit);
    if (captured != EMPTY) {
        uint64_t *cbb = get_bitboard(game, captured);
        if (cbb) *cbb ^= to_bit;
    }

    // Castling: move rook back
    if ((piece == WK || piece == BK) && abs(from_x - to_x) == 2) {
        uint64_t *rbb = (piece == WK) ? &game->wp.rooks : &game->bp.rooks;
        if (to_x > from_x)
            *rbb ^= (1ULL << (from_y * 8 + 5)) | (1ULL << (from_y * 8 + 7));
        else
            *rbb ^= (1ULL << (from_y * 8 + 3)) | (1ULL << (from_y * 8 + 0));
    }

    // En passant: restore the captured pawn
    if ((piece == WP || piece == BP) && from_x != to_x && captured == EMPTY) {
        uint64_t *cbb = (piece == WP) ? &game->bp.pawns : &game->wp.pawns;
        *cbb |= 1ULL << (from_y * 8 + to_x);
    }
}
