#include "engine.h"
#include "../chess_game.h"
#include <stdlib.h>
#include <stdio.h>
int minimax(t_game *game, int depth, bool is_black);
t_move generate_moves(t_game *game)
{
    t_move best = {0};
    int best_score = 1000000;

    uint64_t pieces = game->bp.bishop | game->bp.king | game->bp.knight
                    | game->bp.pawns  | game->bp.queen | game->bp.rooks;

    while (pieces)
    {
        int sq = __builtin_ctzll(pieces);
        int x  = sq % 8;
        int y  = sq / 8;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (!is_valid_destination(game->board, x, y, i, j))
                    continue;

                int piece    = game->board[x][y];
                int captured = game->board[i][j];

                game->old_x = x;
                game->old_y = y;
                move_pieces(game, i, j);
                game->board[i][j] = piece;
                game->board[x][y] = EMPTY;

                int score = minimax(game, DEPTH, false);

                game->board[x][y] = piece;
                game->board[i][j] = captured;
                undo_move(game, piece, x, y, i, j, captured);

                if (score < best_score) {
                    best_score    = score;
                    best.from_x   = x;
                    best.from_y   = y;
                    best.to_x     = i;
                    best.to_y     = j;
                    best.captured = captured;
                }
            }
        }
        pieces &= pieces - 1;
    }
    return best;
}

int minimax(t_game *game, int depth, bool is_black)
{
	if (depth == 0)
	    return (evaluate(game));
	uint64_t pieces;
    int best_score = is_black ? 1000000 : -1000000; 
    if (is_black)
		pieces = game->bp.bishop | game->bp.king | game->bp.knight
                    | game->bp.pawns  | game->bp.queen | game->bp.rooks;
	else
	    pieces = game->wp.bishop | game->wp.king | game->wp.knight
                | game->wp.pawns  | game->wp.queen | game->wp.rooks;

    while (pieces)
    {
        int sq = __builtin_ctzll(pieces);
        int x  = sq % 8;
        int y  = sq / 8;

        for (int i = 0; i < BOARD_SIZE; i++)
        {
            for (int j = 0; j < BOARD_SIZE; j++)
            {
                if (!is_valid_destination(game->board, x, y, i, j))
                    continue;

                int piece    = game->board[x][y];
                int captured = game->board[i][j];

                game->old_x = x;
                game->old_y = y;
                move_pieces(game, i, j);
                game->board[i][j] = piece;
                game->board[x][y] = EMPTY;

                int score = minimax(game, depth - 1, !is_black);

                game->board[x][y] = piece;
                game->board[i][j] = captured;
                undo_move(game, piece, x, y, i, j, captured);
                if (is_black && score < best_score) best_score = score;
                if (!is_black && score > best_score) best_score = score;
			}
        }
        pieces &= pieces - 1;
    }
    return best_score;
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
		int y = sq / 8;
		score += (6 - y) * 10;
		wp &= wp - 1;
	}

	uint64_t bp = game->bp.pawns;
	while (bp) {
		int sq = __builtin_ctzll(bp);
		int y = sq / 8;
		score -= (y - 1) * 10;
		bp &= bp - 1;
	}

	uint64_t wk = game->wp.knight;
	while (wk) {
		int sq = __builtin_ctzll(wk);
		int x = sq % 8;
		int y = sq / 8;
		score += (3 - abs(x - 3)) * 10 + (3 - abs(y - 3)) * 10;
		wk &= wk - 1;
	}

	uint64_t bk = game->bp.knight;
	while (bk) {
		int sq = __builtin_ctzll(bk);
		int x = sq % 8;
		int y = sq / 8;
		score -= (3 - abs(x - 3)) * 10 + (3 - abs(y - 3)) * 10;
		bk &= bk - 1;
	}

	uint64_t wb = game->wp.bishop;
	while (wb) {
		int sq = __builtin_ctzll(wb);
		int x = sq % 8;
		int y = sq / 8;
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
		int x = sq % 8;
		int y = sq / 8;
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
		int y = sq / 8;
		if (y == 1) score += 15;
		if (y == 8) score += 10;
		br &= br - 1;
	}

	uint64_t wr = game->wp.rooks;
	while (wr) {
		int sq = __builtin_ctzll(wr);
		int y = sq / 8;
		if (y == 7) score += 15;
		if (y == 0) score += 10;
		wr &= wr - 1;
	}

	return score;
}
