#ifndef TETRIS_H
#define TETRIS_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE_SQUARE 45
#define MAP_WIDTH 10
#define MAP_HEIGHT 20
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 1000

typedef struct {
    int type;      // 0-6 : I, O, T, S, Z, L, J
    int x;
    int y;
    int rotation; // 0-3
} Piece;

typedef struct {
    char **map;
    Piece current;
    int held_piece;
    int can_hold;
    int frame_counter;
    int score;
    int gamepad_id;
    int game_over;
} GameState;

#endif