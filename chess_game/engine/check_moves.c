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

// ──────────────────────────────────────────────
// Piece-Square Tables (white perspective, y=0=rank8)
// ──────────────────────────────────────────────

static const int pst_pawn[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
     5,  5, 10, 25, 25, 10,  5,  5,
     0,  0,  0, 20, 20,  0,  0,  0,
     5, -5,-10,  0,  0,-10, -5,  5,
     5, 10, 10,-20,-20, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

static const int pst_knight[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

static const int pst_bishop[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

static const int pst_rook[64] = {
     0,  0,  0,  0,  0,  0,  0,  0,
     5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     0,  0,  0,  5,  5,  0,  0,  0
};

static const int pst_queen[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
     -5,  0,  5,  5,  5,  5,  0, -5,
      0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

// King midgame: encourage castling, penalise exposed centre
static const int pst_king_mg[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
     20, 20,  0,  0,  0,  0, 20, 20,
     20, 30, 10,  0,  0, 10, 30, 20
};

// ──────────────────────────────────────────────
// Helpers
// ──────────────────────────────────────────────

static int piece_value(int piece)
{
    switch (piece < 0 ? -piece : piece) {
        case 1: return 100;
        case 3: return 320;
        case 4: return 330;
        case 2: return 500;
        case 5: return 900;
        case 6: return 20000;
    }
    return 0;
}

// Bonus for pawns directly in front of the king (pawn shield)
static int pawn_shield(uint64_t king_bb, uint64_t pawn_bb, bool is_black)
{
    if (!king_bb) return 0;
    int sq = __builtin_ctzll(king_bb);
    int kx = sq % 8, ky = sq / 8;
    int front_y = is_black ? ky + 1 : ky - 1;
    if (front_y < 0 || front_y >= 8) return 0;
    int safety = 0;
    for (int dx = -1; dx <= 1; dx++) {
        int px = kx + dx;
        if (px >= 0 && px < 8 && (pawn_bb & (1ULL << (front_y * 8 + px))))
            safety += 15;
    }
    return safety;
}

// True si le camp a des pièces autres que roi et pions (évite zugzwang en finale)
static bool has_major_pieces(t_game *game, bool is_black)
{
    if (is_black)
        return (game->bp.knight | game->bp.bishop | game->bp.rooks | game->bp.queen) != 0;
    return (game->wp.knight | game->wp.bishop | game->wp.rooks | game->wp.queen) != 0;
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

// ──────────────────────────────────────────────
// Move generation
// ──────────────────────────────────────────────

// All moves, TT best move first, then captures (MVV-LVA), then quiet moves
static int generate_candidates(t_game *game, bool is_black, t_candidate *moves)
{
    int count = 0;
    uint64_t pieces = is_black ? get_black_bb(game) : get_white_bb(game);

    tt_entry_t *hint = &tt[game->hash & (TT_SIZE - 1)];
    int tt_from = (hint->hash == game->hash) ? (int)hint->best_from : -1;
    int tt_to   = (hint->hash == game->hash) ? (int)hint->best_to   : -1;

    game->current_turn = is_black ? 1 : 0;
    while (pieces) {
        int sq = __builtin_ctzll(pieces);
        int x  = sq % 8, y = sq / 8;

        for (int i = 0; i < BOARD_SIZE && count < MAX_MOVES; i++) {
            for (int j = 0; j < BOARD_SIZE && count < MAX_MOVES; j++) {
                if (!is_valid_shape(game, x, y, i, j)) continue;

                int to_sq    = j * 8 + i;
                int victim   = get_piece(game, to_sq);
                int attacker = get_piece(game, sq);
                int score;

                if (sq == tt_from && to_sq == tt_to)
                    score = 2000000; // TT best move first
                else if (victim != EMPTY)
                    score = piece_value(victim) * 10 - piece_value(attacker);
                else
                    score = 0;

                moves[count++] = (t_candidate){ x, y, i, j, score };
            }
        }
        pieces &= pieces - 1;
    }
    return count;
}

// Captures only (for quiescence search)
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
                bool is_ep  = (victim == EMPTY)
                           && (attacker == WP || attacker == BP)
                           && (x != i);

                if (victim == EMPTY && !is_ep) continue;

                int vic_val = (victim != EMPTY)
                    ? piece_value(victim)
                    : piece_value(attacker == WP ? BP : WP);
                int score   = vic_val * 10 - piece_value(attacker);

                moves[count++] = (t_candidate){ x, y, i, j, score };
            }
        }
        pieces &= pieces - 1;
    }
    return count;
}

// Insertion sort descending
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
// ──────────────────────────────────────────────

static int quiescence(t_game *game, bool is_black, int alpha, int beta, int qdepth)
{
    if (qdepth <= 0) return evaluate(game);

    game->current_turn = is_black ? 1 : 0;
    bool in_check = is_in_check(game);

    int stand_pat = evaluate(game);

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

static int minimax(t_game *game, int depth, bool is_black, t_move *out, int alpha, int beta, bool allow_null)
{
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

    // Null move pruning : on passe notre tour et on cherche moins profond.
    // Si même sans jouer on est au-dessus de beta, la position est trop bonne → coupe.
    // Désactivé en échec et en finale (risque de zugzwang).
    if (allow_null && depth >= 3 && !out && has_major_pieces(game, is_black)) {
        game->current_turn = is_black ? 1 : 0;
        if (!is_in_check(game)) {
            int prev_ep = game->ep_square;
            game->ep_square    = -1;
            game->current_turn = is_black ? 0 : 1;
            game->hash        ^= zobrist_black_turn;

            int R          = depth >= 6 ? 3 : 2;
            int null_score = minimax(game, depth - 1 - R, !is_black, NULL, alpha, beta, false);

            game->hash        ^= zobrist_black_turn;
            game->current_turn = is_black ? 1 : 0;
            game->ep_square    = prev_ep;

            if (!is_black && null_score >= beta)  return beta;
            if ( is_black && null_score <= alpha) return alpha;
        }
    }

    int orig_alpha   = alpha;
    int orig_beta    = beta;
    int best_score   = is_black ? 1000000 : -1000000;
    int best_from_sq = -1, best_to_sq = -1;
    bool any_move    = false;

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
        int score = minimax(game, depth - 1, !is_black, NULL, alpha, beta, true);

        undo_move(game, piece, x, y, i, j, captured);
        game->ep_square = prev_ep;
        game->castling  = prev_cast;

        if (!is_black && score > alpha) alpha = score;
        if (is_black  && score < beta)  beta  = score;

        bool better = is_black ? (score < best_score) : (score > best_score);
        if (better) {
            best_score   = score;
            best_from_sq = y * 8 + x;
            best_to_sq   = j * 8 + i;
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
        *entry = (tt_entry_t){ game->hash, depth, best_score, flag,
                               (int16_t)best_from_sq, (int16_t)best_to_sq };
    }
    return best_score;
}

// ──────────────────────────────────────────────
// Public entry point
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

// Iterative deepening: depth 1→DEPTH, reusing TT from each iteration
t_move generate_moves(t_game *game)
{
    t_move best = {0};
    for (int d = 1; d <= DEPTH; d++)
        minimax(game, d, true, &best, INT_MIN, INT_MAX, true);
    if (best.from_x == best.to_x && best.from_y == best.to_y)
        best = find_any_legal_move(game);
    return best;
}

// ──────────────────────────────────────────────
// Static evaluation
// ──────────────────────────────────────────────

int evaluate(t_game *game)
{
    int score = 0;

    // Material
    score += 100   * (__builtin_popcountll(game->wp.pawns)  - __builtin_popcountll(game->bp.pawns));
    score += 320   * (__builtin_popcountll(game->wp.knight) - __builtin_popcountll(game->bp.knight));
    score += 330   * (__builtin_popcountll(game->wp.bishop) - __builtin_popcountll(game->bp.bishop));
    score += 500   * (__builtin_popcountll(game->wp.rooks)  - __builtin_popcountll(game->bp.rooks));
    score += 900   * (__builtin_popcountll(game->wp.queen)  - __builtin_popcountll(game->bp.queen));

    if (game->wp.king == 0) score -= 1000000;
    if (game->bp.king == 0) score += 1000000;

    // Piece-Square Tables
    // White: pst[sq] (y=0 = enemy territory = good)
    // Black: pst[(7-y)*8+x] mirrors the table so black's advancement is rewarded the same way
    uint64_t bb;
    uint64_t wp_bb;
    wp_bb = game->wp.pawns;
    while (wp_bb) { int sq = __builtin_ctzll(wp_bb); score += pst_pawn[sq];    wp_bb &= wp_bb-1; }
    wp_bb = game->wp.knight;
    while (wp_bb) { int sq = __builtin_ctzll(wp_bb); score += pst_knight[sq];  wp_bb &= wp_bb-1; }
    wp_bb = game->wp.bishop;
    while (wp_bb) { int sq = __builtin_ctzll(wp_bb); score += pst_bishop[sq];  wp_bb &= wp_bb-1; }
    wp_bb = game->wp.rooks;
    while (wp_bb) { int sq = __builtin_ctzll(wp_bb); score += pst_rook[sq];    wp_bb &= wp_bb-1; }
    wp_bb = game->wp.queen;
    while (wp_bb) { int sq = __builtin_ctzll(wp_bb); score += pst_queen[sq];   wp_bb &= wp_bb-1; }
    wp_bb = game->wp.king;
    while (wp_bb) { int sq = __builtin_ctzll(wp_bb); score += pst_king_mg[sq]; wp_bb &= wp_bb-1; }

    bb = game->bp.pawns;
    while (bb) { int sq = __builtin_ctzll(bb); score -= pst_pawn[   (7-(sq/8))*8+(sq%8)]; bb &= bb-1; }
    bb = game->bp.knight;
    while (bb) { int sq = __builtin_ctzll(bb); score -= pst_knight[ (7-(sq/8))*8+(sq%8)]; bb &= bb-1; }
    bb = game->bp.bishop;
    while (bb) { int sq = __builtin_ctzll(bb); score -= pst_bishop[ (7-(sq/8))*8+(sq%8)]; bb &= bb-1; }
    bb = game->bp.rooks;
    while (bb) { int sq = __builtin_ctzll(bb); score -= pst_rook[   (7-(sq/8))*8+(sq%8)]; bb &= bb-1; }
    bb = game->bp.queen;
    while (bb) { int sq = __builtin_ctzll(bb); score -= pst_queen[  (7-(sq/8))*8+(sq%8)]; bb &= bb-1; }
    bb = game->bp.king;
    while (bb) { int sq = __builtin_ctzll(bb); score -= pst_king_mg[(7-(sq/8))*8+(sq%8)]; bb &= bb-1; }

    // King safety: pawn shield
    score += pawn_shield(game->wp.king, game->wp.pawns, false);
    score -= pawn_shield(game->bp.king, game->bp.pawns, true);

    return score;
}
