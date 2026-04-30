#ifndef ENGINE_H
#define ENGINE_H

#include "../chess_game.h"
#include <stddef.h>

#define DEPTH 4

void set_pieces(t_bitboards *wp, t_bitboards *bp);
int evaluate(t_game *game);

#endif
