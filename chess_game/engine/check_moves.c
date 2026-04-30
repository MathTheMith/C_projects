#include "engine.h"
#include "../chess_game.h"
#include <stdlib.h>

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
                if (!is_valid_destination(game, x, y, i, j))
                    continue;

                int piece    = get_piece(game, y * 8 + x);
                int captured = get_piece(game, j * 8 + i);

                game->old_x = x;
                game->old_y = y;
                move_pieces(game, i, j);

                game->current_turn = is_black ? 0 : 1;
                int score = minimax(game, depth - 1, !is_black, NULL);

                undo_move(game, piece, x, y, i, j, captured);

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

t_move generate_moves(t_game *game)
{
    t_move best = {0};
    minimax(game, DEPTH, true, &best);
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
