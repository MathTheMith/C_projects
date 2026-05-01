#include "engine.h"
#include "../chess_game.h"
#include <stdlib.h>

static bool is_valid_shape(t_game *game, int ox, int oy, int nx, int ny)
{
    if (ox == nx && oy == ny) return false;
    if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) return false;
    int piece = get_piece(game, oy * 8 + ox);
    if (piece == WR || piece == BR) return is_valid_rook_move(game, ox, oy, nx, ny);
    if (piece == WB || piece == BB) return is_valid_bishop_move(game, ox, oy, nx, ny);
    if (piece == WN || piece == BN) return is_valid_knight_move(game, ox, oy, nx, ny);
    if (piece == WQ || piece == BQ) return is_valid_queen_move(game, ox, oy, nx, ny);
    if (piece == WK || piece == BK) return is_valid_king_move(game, ox, oy, nx, ny);
    if (piece == WP) return is_valid_wpawn_move(game, ox, oy, nx, ny);
    if (piece == BP) return is_valid_bpawn_move(game, ox, oy, nx, ny);
    return false;
}

static int minimax(t_game *game, int depth, bool is_black, t_move *out)
{
    if (depth == 0)
        return evaluate(game);

    uint64_t pieces = is_black ? get_black_bb(game) : get_white_bb(game);
    int best_score  = is_black ? 1000000 : -1000000;

    while (pieces)
    {
        int sq = __builtin_ctzll(pieces);
        int x  = sq % 8;
        int y  = sq / 8;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                game->current_turn = is_black ? 1 : 0;
                if (!is_valid_shape(game, x, y, i, j))
                    continue;

                int piece     = get_piece(game, y * 8 + x);
                int captured  = get_piece(game, j * 8 + i);
                int prev_ep   = game->ep_square;
                uint8_t prev_cast = game->castling;

                game->old_x = x;
                game->old_y = y;
                move_pieces(game, i, j);

                if (is_in_check(game)) {
                    undo_move(game, piece, x, y, i, j, captured);
                    game->ep_square = prev_ep;
                    game->castling  = prev_cast;
                    continue;
                }

                game->current_turn = is_black ? 0 : 1;
                int score = minimax(game, depth - 1, !is_black, NULL);

                undo_move(game, piece, x, y, i, j, captured);
                game->ep_square = prev_ep;
                game->castling  = prev_cast;

                bool better = is_black ? (score < best_score) : (score > best_score);
                if (better) {
                    best_score = score;
                    if (out) {
                        out->from_x   = x;
                        out->from_y   = y;
                        out->to_x     = i;
                        out->to_y     = j;
                        out->captured = captured;
                    }
                }
            }
        }
        pieces &= pieces - 1;
    }
    return best_score;
}

static t_move find_any_legal_move(t_game *game)
{
    t_move fallback = {0};
    uint64_t pieces = get_black_bb(game);
    while (pieces) {
        int sq = __builtin_ctzll(pieces);
        int fx = sq % 8, fy = sq / 8;
        for (int tx = 0; tx < BOARD_SIZE; tx++)
            for (int ty = 0; ty < BOARD_SIZE; ty++)
                if (is_valid_destination(game, fx, fy, tx, ty)) {
                    fallback.from_x = fx; fallback.from_y = fy;
                    fallback.to_x   = tx; fallback.to_y   = ty;
                    fallback.captured = get_piece(game, ty * 8 + tx);
                    return fallback;
                }
        pieces &= pieces - 1;
    }
    return fallback;
}

t_move generate_moves(t_game *game)
{
    t_move best = {0};
    minimax(game, DEPTH, true, &best);
    if (best.from_x == best.to_x && best.from_y == best.to_y)
        best = find_any_legal_move(game);
    return best;
}

int evaluate(t_game *game)
{
    int score;

    score  = 100  * (__builtin_popcountll(game->wp.pawns)  - __builtin_popcountll(game->bp.pawns));
    score += 300  * (__builtin_popcountll(game->wp.bishop) - __builtin_popcountll(game->bp.bishop));
    score += 1000 * (__builtin_popcountll(game->wp.king)   - __builtin_popcountll(game->bp.king));
    score += 300  * (__builtin_popcountll(game->wp.knight) - __builtin_popcountll(game->bp.knight));
    score += 500  * (__builtin_popcountll(game->wp.rooks)  - __builtin_popcountll(game->bp.rooks));
    score += 900  * (__builtin_popcountll(game->wp.queen)  - __builtin_popcountll(game->bp.queen));

    if (game->wp.king == 0) score -= 1000000;
    if (game->bp.king == 0) score += 1000000;

    uint64_t wp = game->wp.pawns;
    while (wp) {
        int sq = __builtin_ctzll(wp);
        int y  = sq / 8;
        score += (6 - y) * 10;
        wp &= wp - 1;
    }

    uint64_t bp = game->bp.pawns;
    while (bp) {
        int sq = __builtin_ctzll(bp);
        int y  = sq / 8;
        score -= (y - 1) * 10;
        bp &= bp - 1;
    }

    uint64_t wk = game->wp.knight;
    while (wk) {
        int sq = __builtin_ctzll(wk);
        int x  = sq % 8;
        int y  = sq / 8;
        score += (3 - abs(x - 3)) * 10 + (3 - abs(y - 3)) * 10;
        wk &= wk - 1;
    }

    uint64_t bk = game->bp.knight;
    while (bk) {
        int sq = __builtin_ctzll(bk);
        int x  = sq % 8;
        int y  = sq / 8;
        score -= (3 - abs(x - 3)) * 10 + (3 - abs(y - 3)) * 10;
        bk &= bk - 1;
    }

    uint64_t wb = game->wp.bishop;
    while (wb) {
        int sq = __builtin_ctzll(wb);
        int x  = sq % 8;
        int y  = sq / 8;
        int mobility = (7 - x < 7 - y ? 7 - x : 7 - y)
                     + (x < y ? x : y)
                     + (x < 7 - y ? x : 7 - y)
                     + (7 - x < y ? 7 - x : y);
        score += mobility * 5;
        wb &= wb - 1;
    }

    uint64_t bb = game->bp.bishop;
    while (bb) {
        int sq = __builtin_ctzll(bb);
        int x  = sq % 8;
        int y  = sq / 8;
        int mobility = (7 - x < 7 - y ? 7 - x : 7 - y)
                     + (x < y ? x : y)
                     + (x < 7 - y ? x : 7 - y)
                     + (7 - x < y ? 7 - x : y);
        score -= mobility * 5;
        bb &= bb - 1;
    }

    uint64_t br = game->bp.rooks;
    while (br) {
        int sq = __builtin_ctzll(br);
        int y  = sq / 8;
        if (y == 1) score -= 15;
        if (y == 0) score -= 10;
        br &= br - 1;
    }

    uint64_t wr = game->wp.rooks;
    while (wr) {
        int sq = __builtin_ctzll(wr);
        int y  = sq / 8;
        if (y == 7) score += 15;
        if (y == 0) score += 10;
        wr &= wr - 1;
    }

    return score;
}
