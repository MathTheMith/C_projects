#ifndef GAME_H
#define GAME_H

#include "tetris.h"

// Game initialization and cleanup
GameState* init_game(void);
void free_game(GameState *state);

// Game loop
void update_game(GameState *state);
int check_game_over(GameState *state);
void draw_game_over_screen(int score);

// Input handling
void handle_input(GameState *state);

#endif