#include "engine.h"
#include "../chess_game.h"
#include <stdlib.h>
#include <limits.h>

static tt_entry_t tt[TT_SIZE];

#define MAX_MOVES 256

typedef struct {
    int from_x, from_y, to_x, to_y;
    int order_score;
} t_candidate;

static int piece_value(int piece)
{
    switch (piece < 0 ? -piece : piece) {
        case 1: return 100;
        case 3: return 300;
        case 4: return 300;
        case 2: return 500;
        case 5: return 900;
        case 6: return 1000;
    }
    return 0;
}

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

// Génère tous les coups, triés captures en tête (MVV-LVA)
static int generate_candidates(t_game *game, bool is_black, t_candidate *moves)
{
    int count = 0;
    uint64_t pieces = is_black ? get_black_bb(game) : get_white_bb(game);

    game->current_turn = is_black ? 1 : 0;
    while (pieces) {
        int sq = __builtin_ctzll(pieces);
        int x  = sq % 8, y = sq / 8;

        for (int i = 0; i < BOARD_SIZE && count < MAX_MOVES; i++) {
            for (int j = 0; j < BOARD_SIZE && count < MAX_MOVES; j++) {
                if (!is_valid_shape(game, x, y, i, j)) continue;

                int victim   = get_piece(game, j * 8 + i);
                int attacker = get_piece(game, sq);
                int score = (victim != EMPTY)
                    ? piece_value(victim) * 10 - piece_value(attacker)
                    : 0;

                moves[count++] = (t_candidate){ x, y, i, j, score };
            }
        }
        pieces &= pieces - 1;
    }
    return count;
}

// Génère uniquement les captures (pour la quiescence search)
static int generate_captures(t_game *game, bool is_black, t_candidate *moves)
{
    int count = 0;
    uint64_t pieces = is_black ? get_black_bb(game) : get_white_bb(game);

    game->current_turn = is_black ? 1 : 0;
    while (pieces) {
        int sq       = __builtin_ctzll(pieces);
        int x        = sq % 8, y = sq / 8;
        int attacker = get_piece(game, sq);

        for (int i = 0; i < BOARD_SIZE && count < MAX_MOVES; i++) {
            for (int j = 0; j < BOARD_SIZE && count < MAX_MOVES; j++) {
                if (!is_valid_shape(game, x, y, i, j)) continue;

                int victim  = get_piece(game, j * 8 + i);
                // En passant : pion se déplace en diagonale vers une case vide
                bool is_ep  = (victim == EMPTY)
                           && (attacker == WP || attacker == BP)
                           && (x != i);

                if (victim == EMPTY && !is_ep) continue;

                int vic_val = (victim != EMPTY)
                    ? piece_value(victim)
                    : piece_value(attacker == WP ? BP : WP); // pion capturé en ep
                int score   = vic_val * 10 - piece_value(attacker);

                moves[count++] = (t_candidate){ x, y, i, j, score };
            }
        }
        pieces &= pieces - 1;
    }
    return count;
}

// Tri par insertion décroissant — rapide pour ~20-50 coups
static void sort_candidates(t_candidate *moves, int count)
{
    for (int i = 1; i < count; i++) {
        t_candidate key = moves[i];
        int j = i - 1;
        while (j >= 0 && moves[j].order_score < key.order_score) {
            moves[j + 1] = moves[j];
            j--;
        }
        moves[j + 1] = key;
    }
}

// ──────────────────────────────────────────────
// Quiescence search
// - Si en échec : explore tous les coups (forçé)
// - Sinon       : explore uniquement les captures
// Limité à QDEPTH niveaux.
// ──────────────────────────────────────────────

static int quiescence(t_game *game, bool is_black, int alpha, int beta, int qdepth)
{
    if (qdepth <= 0) return evaluate(game);

    // Vérifie si le camp courant est en échec
    game->current_turn = is_black ? 1 : 0;
    bool in_check = is_in_check(game);

    int stand_pat = evaluate(game);

    // Stand-pat invalide quand on est en échec (on doit bouger)
    if (!in_check) {
        if (!is_black) {
            if (stand_pat >= beta)  return beta;
            if (stand_pat > alpha)  alpha = stand_pat;
        } else {
            if (stand_pat <= alpha) return alpha;
            if (stand_pat < beta)   beta  = stand_pat;
        }
    }

    t_candidate moves[MAX_MOVES];
    // En échec : tous les coups légaux ; sinon : uniquement les captures
    int n = in_check
        ? generate_candidates(game, is_black, moves)
        : generate_captures(game, is_black, moves);
    sort_candidates(moves, n);

    for (int m = 0; m < n; m++) {
        int x = moves[m].from_x, y = moves[m].from_y;
        int i = moves[m].to_x,   j = moves[m].to_y;

        game->current_turn = is_black ? 1 : 0;
        int piece         = get_piece(game, y * 8 + x);
        int captured      = get_piece(game, j * 8 + i);
        int prev_ep       = game->ep_square;
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
        int score = quiescence(game, !is_black, alpha, beta, qdepth - 1);

        undo_move(game, piece, x, y, i, j, captured);
        game->ep_square = prev_ep;
        game->castling  = prev_cast;

        if (!is_black) {
            if (score >= beta)  return beta;
            if (score > alpha)  alpha = score;
        } else {
            if (score <= alpha) return alpha;
            if (score < beta)   beta  = score;
        }
    }

    return is_black ? beta : alpha;
}

// ──────────────────────────────────────────────
// Minimax + alpha-beta + TT + move ordering
// ──────────────────────────────────────────────

static int minimax(t_game *game, int depth, bool is_black, t_move *out, int alpha, int beta)
{
    // TT probe — ignoré à la racine (out != NULL)
    tt_entry_t *entry = &tt[game->hash & (TT_SIZE - 1)];
    if (!out && entry->hash == game->hash && entry->depth >= depth) {
        if (entry->flag == TT_EXACT) return entry->score;
        if (entry->flag == TT_ALPHA && entry->score <= alpha) return alpha;
        if (entry->flag == TT_BETA  && entry->score >= beta)  return beta;
    }

    if (depth == 0)
        return quiescence(game, is_black, alpha, beta, QDEPTH);
    if (beta <= alpha)
        return evaluate(game);

    int orig_alpha = alpha;
    int orig_beta  = beta;
    int best_score = is_black ? 1000000 : -1000000;
    bool any_move  = false;

    t_candidate moves[MAX_MOVES];
    int n = generate_candidates(game, is_black, moves);
    sort_candidates(moves, n);

    for (int m = 0; m < n; m++) {
        int x = moves[m].from_x, y = moves[m].from_y;
        int i = moves[m].to_x,   j = moves[m].to_y;

        game->current_turn = is_black ? 1 : 0;
        int piece         = get_piece(game, y * 8 + x);
        int captured      = get_piece(game, j * 8 + i);
        int prev_ep       = game->ep_square;
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
        any_move = true;
        int score = minimax(game, depth - 1, !is_black, NULL, alpha, beta);

        undo_move(game, piece, x, y, i, j, captured);
        game->ep_square = prev_ep;
        game->castling  = prev_cast;

        if (!is_black && score > alpha) alpha = score;
        if (is_black  && score < beta)  beta  = score;

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

        if (beta <= alpha) break;
    }

    if (any_move) {
        tt_flag_t flag;
        if      (best_score <= orig_alpha) flag = TT_ALPHA;
        else if (best_score >= orig_beta)  flag = TT_BETA;
        else                               flag = TT_EXACT;
        *entry = (tt_entry_t){ game->hash, depth, best_score, flag };
    }
    return best_score;
}

// ──────────────────────────────────────────────
// Entrée publique
// ──────────────────────────────────────────────

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
    minimax(game, DEPTH, true, &best, INT_MIN, INT_MAX);
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
