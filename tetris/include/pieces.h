#ifndef PIECES_H
#define PIECES_H

#include "tetris.h"

// Collision detection
int can_move(char **map, Piece *piece, char direction);
int is_position_valid(char **map, Piece *piece);

// Piece locking
int lock_piece(char **map, Piece *piece);

// Piece rotation
void rotate_piece(char **map, Piece *piece, int direction);

// Drawing
void draw_piece(Piece *piece);
void draw_ghost_piece(char **map, Piece *piece);
void draw_cube(int x, int y, Color color);
void draw_piece_preview(int type, int x, int y);

#endif