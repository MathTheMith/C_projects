#ifndef ENGINE_H
#define ENGINE_H

#include "../chess_game.h"
#include <stddef.h>

#define DEPTH   6
#define QDEPTH  4
#define TT_SIZE (1 << 20)

typedef enum { TT_EXACT = 0, TT_ALPHA, TT_BETA } tt_flag_t;

typedef struct {
    uint64_t  hash;
    int       depth;
    int       score;
    tt_flag_t flag;
    int16_t   best_from;
    int16_t   best_to;
} tt_entry_t;

static inline int piece_to_index(int piece) {
    return piece > 0 ? piece - 1 : -piece + 5;
}

extern uint64_t zobrist_table[12][64];
extern uint64_t zobrist_black_turn;

void     set_pieces(t_bitboards *wp, t_bitboards *bp);
void     init_zobrist(void);
uint64_t compute_hash(t_game *game);
int      evaluate(t_game *game);

#endif
