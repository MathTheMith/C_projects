#include "chess_game.h"
#include <stdlib.h>

bool can_capture(t_game *game, int from_sq, int to_sq)
{
    uint64_t from_mask = 1ULL << from_sq;
    uint64_t to_mask   = 1ULL << to_sq;
    uint64_t wp = get_white_bb(game);
    uint64_t bp = get_black_bb(game);

    if ((wp & from_mask) && (bp & to_mask)) return true;
    if ((bp & from_mask) && (wp & to_mask)) return true;
    return false;
}

bool is_piece_on_square(t_game *game, int x, int y)
{
    return (get_occupied_bb(game) & (1ULL << (y * 8 + x))) != 0;
}

bool is_valid_rook_move(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    if (old_x != new_x && old_y != new_y) return false;

    uint64_t occ = get_occupied_bb(game);
    int step_x = (new_x > old_x) ? 1 : (new_x < old_x) ? -1 : 0;
    int step_y = (new_y > old_y) ? 1 : (new_y < old_y) ? -1 : 0;
    int x = old_x + step_x;
    int y = old_y + step_y;

    while (x != new_x || y != new_y) {
        if (occ & (1ULL << (y * 8 + x))) return false;
        x += step_x;
        y += step_y;
    }

    uint64_t dest = 1ULL << (new_y * 8 + new_x);
    if (!(occ & dest)) return true;
    return can_capture(game, old_y * 8 + old_x, new_y * 8 + new_x);
}

bool is_valid_bishop_move(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    int dx = new_x - old_x;
    int dy = new_y - old_y;

    if (abs(dx) != abs(dy)) return false;

    uint64_t occ = get_occupied_bb(game);
    int step_x = (dx > 0) ? 1 : -1;
    int step_y = (dy > 0) ? 1 : -1;
    int x = old_x + step_x;
    int y = old_y + step_y;

    while (x != new_x && y != new_y) {
        if (occ & (1ULL << (y * 8 + x))) return false;
        x += step_x;
        y += step_y;
    }

    uint64_t dest = 1ULL << (new_y * 8 + new_x);
    if (!(occ & dest)) return true;
    return can_capture(game, old_y * 8 + old_x, new_y * 8 + new_x);
}

bool is_valid_queen_move(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    return is_valid_rook_move(game, old_x, old_y, new_x, new_y)
        || is_valid_bishop_move(game, old_x, old_y, new_x, new_y);
}

bool is_valid_knight_move(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    int dx = abs(new_x - old_x);
    int dy = abs(new_y - old_y);

    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) return false;

    uint64_t dest = 1ULL << (new_y * 8 + new_x);
    if (!(get_occupied_bb(game) & dest)) return true;
    return can_capture(game, old_y * 8 + old_x, new_y * 8 + new_x);
}

bool is_valid_king_move(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    int dx = abs(new_x - old_x);
    int dy = abs(new_y - old_y);

    if (dx <= 1 && dy <= 1 && !(dx == 0 && dy == 0)) {
        uint64_t dest = 1ULL << (new_y * 8 + new_x);
        if (!(get_occupied_bb(game) & dest)) return true;
        return can_capture(game, old_y * 8 + old_x, new_y * 8 + new_x);
    }

    if (dx == 2 && dy == 0) {
        if (is_in_check(game)) return false;
        int piece = get_piece(game, old_y * 8 + old_x);
        uint64_t occ = get_occupied_bb(game);
        if (piece == WK && old_x == 4 && old_y == 7 && new_x == 6 && (game->castling & 0x1)) {
            if (occ & ((1ULL << (7*8+5)) | (1ULL << (7*8+6)))) return false;
            return true;
        }
        if (piece == WK && old_x == 4 && old_y == 7 && new_x == 2 && (game->castling & 0x2)) {
            if (occ & ((1ULL << (7*8+3)) | (1ULL << (7*8+2)) | (1ULL << (7*8+1)))) return false;
            return true;
        }
        if (piece == BK && old_x == 4 && old_y == 0 && new_x == 6 && (game->castling & 0x4)) {
            if (occ & ((1ULL << (0*8+5)) | (1ULL << (0*8+6)))) return false;
            return true;
        }
        if (piece == BK && old_x == 4 && old_y == 0 && new_x == 2 && (game->castling & 0x8)) {
            if (occ & ((1ULL << (0*8+3)) | (1ULL << (0*8+2)) | (1ULL << (0*8+1)))) return false;
            return true;
        }
    }
    return false;
}

bool is_valid_wpawn_move(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    uint64_t occ = get_occupied_bb(game);

    if (new_x == old_x) {
        if (new_y == old_y - 1 && !(occ & (1ULL << (new_y * 8 + new_x))))
            return true;
        if (old_y == 6 && new_y == old_y - 2
            && !(occ & (1ULL << (new_y * 8 + new_x)))
            && !(occ & (1ULL << ((old_y - 1) * 8 + old_x))))
            return true;
    }
    if ((new_x == old_x + 1 || new_x == old_x - 1) && new_y == old_y - 1) {
        if (get_black_bb(game) & (1ULL << (new_y * 8 + new_x)))
            return true;
        if (game->ep_square == new_y * 8 + new_x)
            return true;
    }
    return false;
}

bool is_valid_bpawn_move(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    uint64_t occ = get_occupied_bb(game);

    if (new_x == old_x) {
        if (new_y == old_y + 1 && !(occ & (1ULL << (new_y * 8 + new_x))))
            return true;
        if (old_y == 1 && new_y == old_y + 2
            && !(occ & (1ULL << (new_y * 8 + new_x)))
            && !(occ & (1ULL << ((old_y + 1) * 8 + old_x))))
            return true;
    }
    if ((new_x == old_x + 1 || new_x == old_x - 1) && new_y == old_y + 1) {
        if (get_white_bb(game) & (1ULL << (new_y * 8 + new_x)))
            return true;
        if (game->ep_square == new_y * 8 + new_x)
            return true;
    }
    return false;
}

bool is_in_check(t_game *game)
{
    uint64_t king_bb = (game->current_turn == 0) ? game->wp.king : game->bp.king;
    if (!king_bb) return false;

    int king_sq = __builtin_ctzll(king_bb);
    int kx = king_sq % 8;
    int ky = king_sq / 8;

    uint64_t occ            = get_occupied_bb(game);
    uint64_t enemy_rooks    = (game->current_turn == 0) ? game->bp.rooks  : game->wp.rooks;
    uint64_t enemy_bishops  = (game->current_turn == 0) ? game->bp.bishop : game->wp.bishop;
    uint64_t enemy_queens   = (game->current_turn == 0) ? game->bp.queen  : game->wp.queen;
    uint64_t enemy_knights  = (game->current_turn == 0) ? game->bp.knight : game->wp.knight;
    uint64_t enemy_pawns    = (game->current_turn == 0) ? game->bp.pawns  : game->wp.pawns;
    uint64_t enemy_king     = (game->current_turn == 0) ? game->bp.king   : game->wp.king;

    int straight[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int d = 0; d < 4; d++) {
        int x = kx + straight[d][0];
        int y = ky + straight[d][1];
        while (x >= 0 && x < 8 && y >= 0 && y < 8) {
            uint64_t mask = 1ULL << (y * 8 + x);
            if (occ & mask) {
                if ((enemy_rooks | enemy_queens) & mask) return true;
                break;
            }
            x += straight[d][0];
            y += straight[d][1];
        }
    }

    int diag[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (int d = 0; d < 4; d++) {
        int x = kx + diag[d][0];
        int y = ky + diag[d][1];
        while (x >= 0 && x < 8 && y >= 0 && y < 8) {
            uint64_t mask = 1ULL << (y * 8 + x);
            if (occ & mask) {
                if ((enemy_bishops | enemy_queens) & mask) return true;
                break;
            }
            x += diag[d][0];
            y += diag[d][1];
        }
    }

    int knight_offsets[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
    for (int i = 0; i < 8; i++) {
        int x = kx + knight_offsets[i][0];
        int y = ky + knight_offsets[i][1];
        if (x >= 0 && x < 8 && y >= 0 && y < 8)
            if (enemy_knights & (1ULL << (y * 8 + x))) return true;
    }

    // Enemy king proximity
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int x = kx + dx, y = ky + dy;
            if (x >= 0 && x < 8 && y >= 0 && y < 8)
                if (enemy_king & (1ULL << (y * 8 + x))) return true;
        }
    }

    int pawn_y = (game->current_turn == 0) ? ky - 1 : ky + 1;
    if (pawn_y >= 0 && pawn_y < 8) {
        if (kx - 1 >= 0 && (enemy_pawns & (1ULL << (pawn_y * 8 + (kx - 1))))) return true;
        if (kx + 1 <  8 && (enemy_pawns & (1ULL << (pawn_y * 8 + (kx + 1))))) return true;
    }

    return false;
}

bool is_valid_destination(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    if (old_x == new_x && old_y == new_y) return false;
    if (new_x < 0 || new_x >= BOARD_SIZE || new_y < 0 || new_y >= BOARD_SIZE) return false;

    int piece = get_piece(game, old_y * 8 + old_x);
    bool valid = false;

    if (piece == WR || piece == BR) valid = is_valid_rook_move(game, old_x, old_y, new_x, new_y);
    if (piece == WB || piece == BB) valid = is_valid_bishop_move(game, old_x, old_y, new_x, new_y);
    if (piece == WN || piece == BN) valid = is_valid_knight_move(game, old_x, old_y, new_x, new_y);
    if (piece == WQ || piece == BQ) valid = is_valid_queen_move(game, old_x, old_y, new_x, new_y);
    if (piece == WK || piece == BK) valid = is_valid_king_move(game, old_x, old_y, new_x, new_y);
    if (piece == WP) valid = is_valid_wpawn_move(game, old_x, old_y, new_x, new_y);
    if (piece == BP) valid = is_valid_bpawn_move(game, old_x, old_y, new_x, new_y);

    if (!valid) return false;

    // Castling: king must not be in check now, and must not pass through an attacked square
    if ((piece == WK || piece == BK) && abs(new_x - old_x) == 2) {
        if (is_in_check(game)) return false;
        int mid_x = (old_x + new_x) / 2;
        uint64_t *king_bb = (piece == WK) ? &game->wp.king : &game->bp.king;
        uint64_t from_bit = 1ULL << (old_y * 8 + old_x);
        uint64_t mid_bit  = 1ULL << (old_y * 8 + mid_x);
        *king_bb ^= (from_bit | mid_bit);
        bool mid_attacked = is_in_check(game);
        *king_bb ^= (from_bit | mid_bit);
        if (mid_attacked) return false;
    }

    int prev_ep       = game->ep_square;
    uint8_t prev_cast = game->castling;
    int captured      = get_piece(game, new_y * 8 + new_x);
    int saved_ox      = game->old_x;
    int saved_oy      = game->old_y;
    game->old_x       = old_x;
    game->old_y       = old_y;
    move_pieces(game, new_x, new_y);

    bool in_check = is_in_check(game);

    undo_move(game, piece, old_x, old_y, new_x, new_y, captured);
    game->old_x     = saved_ox;
    game->old_y     = saved_oy;
    game->ep_square = prev_ep;
    game->castling  = prev_cast;

    return !in_check;
}

bool has_legal_moves(t_game *game)
{
    uint64_t pieces = (game->current_turn == 0) ? get_white_bb(game) : get_black_bb(game);
    while (pieces) {
        int sq = __builtin_ctzll(pieces);
        int fx = sq % 8;
        int fy = sq / 8;
        for (int tx = 0; tx < BOARD_SIZE; tx++)
            for (int ty = 0; ty < BOARD_SIZE; ty++)
                if (is_valid_destination(game, fx, fy, tx, ty))
                    return true;
        pieces &= pieces - 1;
    }
    return false;
}
