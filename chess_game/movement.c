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

    if (dx > 1 || dy > 1 || (dx == 0 && dy == 0)) return false;

    uint64_t dest = 1ULL << (new_y * 8 + new_x);
    if (!(get_occupied_bb(game) & dest)) return true;
    return can_capture(game, old_y * 8 + old_x, new_y * 8 + new_x);
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
    }
    return false;
}

bool is_valid_destination(t_game *game, int old_x, int old_y, int new_x, int new_y)
{
    if (old_x == new_x && old_y == new_y) return false;
    if (new_x < 0 || new_x >= BOARD_SIZE || new_y < 0 || new_y >= BOARD_SIZE) return false;

    int piece = get_piece(game, old_y * 8 + old_x);

    if (piece == WR || piece == BR) return is_valid_rook_move(game, old_x, old_y, new_x, new_y);
    if (piece == WB || piece == BB) return is_valid_bishop_move(game, old_x, old_y, new_x, new_y);
    if (piece == WN || piece == BN) return is_valid_knight_move(game, old_x, old_y, new_x, new_y);
    if (piece == WQ || piece == BQ) return is_valid_queen_move(game, old_x, old_y, new_x, new_y);
    if (piece == WK || piece == BK) return is_valid_king_move(game, old_x, old_y, new_x, new_y);
    if (piece == WP) return is_valid_wpawn_move(game, old_x, old_y, new_x, new_y);
    if (piece == BP) return is_valid_bpawn_move(game, old_x, old_y, new_x, new_y);
    return false;
}
