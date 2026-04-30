#include "raylib.h"
#include "chess_game.h"

bool is_valid_move(t_game *game, int x, int y)
{
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return false;
    return !(get_occupied_bb(game) & (1ULL << (y * 8 + x)));
}

static void draw_capture(int x, int y)
{
    Color captureGrey = (Color){80, 80, 80, 75};
    DrawRectangle(x * 100, y * 100, 100, 100, captureGrey);
}

void show_possibles_moves(t_game *game, int x, int y)
{
    int piece = get_piece(game, y * 8 + x);
    if (piece == WR || piece == BR) show_rook_moves(game, x, y);
    if (piece == WB || piece == BB) show_bishop_moves(game, x, y);
    if (piece == WN || piece == BN) show_knight_moves(game, x, y);
    if (piece == WQ || piece == BQ) show_queen_moves(game, x, y);
    if (piece == WK || piece == BK) show_king_moves(game, x, y);
    if (piece == WP) show_wpawns_moves(game, x, y);
    if (piece == BP) show_bpawns_moves(game, x, y);
}

void show_wpawns_moves(t_game *game, int x, int y)
{
    int i;
    Color lightBlack = (Color){22, 21, 17, 100};
    int from_sq = y * 8 + x;
    i = y - 1;
    if (y == 6 && is_valid_move(game, x, i)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i--;
        if (is_valid_move(game, x, i))
            DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
    } else if (is_valid_move(game, x, i)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
    }
    if (x + 1 < BOARD_SIZE && y - 1 >= 0 && can_capture(game, from_sq, (y - 1) * 8 + (x + 1)))
        draw_capture(x + 1, y - 1);
    if (x - 1 >= 0 && y - 1 >= 0 && can_capture(game, from_sq, (y - 1) * 8 + (x - 1)))
        draw_capture(x - 1, y - 1);
}

void show_bpawns_moves(t_game *game, int x, int y)
{
    int i;
    Color lightBlack = (Color){22, 21, 17, 100};
    int from_sq = y * 8 + x;
    i = y + 1;
    if (y == 1 && is_valid_move(game, x, i)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i++;
        if (is_valid_move(game, x, i))
            DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
    } else if (is_valid_move(game, x, i)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
    }
    if (x + 1 < BOARD_SIZE && y + 1 < BOARD_SIZE && can_capture(game, from_sq, (y + 1) * 8 + (x + 1)))
        draw_capture(x + 1, y + 1);
    if (x - 1 >= 0 && y + 1 < BOARD_SIZE && can_capture(game, from_sq, (y + 1) * 8 + (x - 1)))
        draw_capture(x - 1, y + 1);
}

void show_rook_moves(t_game *game, int x, int y)
{
    int i;
    Color lightBlack = (Color){22, 21, 17, 100};
    int from_sq = y * 8 + x;

    i = x + 1;
    while (i < BOARD_SIZE && is_valid_move(game, i, y)) {
        DrawCircle(i * 100 + 50, y * 100 + 50, 15, lightBlack);
        i++;
    }
    if (i < BOARD_SIZE && can_capture(game, from_sq, y * 8 + i))
        draw_capture(i, y);

    i = x - 1;
    while (i >= 0 && is_valid_move(game, i, y)) {
        DrawCircle(i * 100 + 50, y * 100 + 50, 15, lightBlack);
        i--;
    }
    if (i >= 0 && can_capture(game, from_sq, y * 8 + i))
        draw_capture(i, y);

    i = y + 1;
    while (i < BOARD_SIZE && is_valid_move(game, x, i)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i++;
    }
    if (i < BOARD_SIZE && can_capture(game, from_sq, i * 8 + x))
        draw_capture(x, i);

    i = y - 1;
    while (i >= 0 && is_valid_move(game, x, i)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i--;
    }
    if (i >= 0 && can_capture(game, from_sq, i * 8 + x))
        draw_capture(x, i);
}

void show_bishop_moves(t_game *game, int x, int y)
{
    int i = x + 1, j = y + 1;
    Color lightBlack = (Color){22, 21, 17, 100};
    int from_sq = y * 8 + x;

    while (is_valid_move(game, i, j)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i++; j++;
    }
    if (i < BOARD_SIZE && j < BOARD_SIZE && can_capture(game, from_sq, j * 8 + i))
        draw_capture(i, j);

    i = x - 1; j = y - 1;
    while (is_valid_move(game, i, j)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i--; j--;
    }
    if (i >= 0 && j >= 0 && can_capture(game, from_sq, j * 8 + i))
        draw_capture(i, j);

    i = x - 1; j = y + 1;
    while (is_valid_move(game, i, j)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i--; j++;
    }
    if (i >= 0 && j < BOARD_SIZE && can_capture(game, from_sq, j * 8 + i))
        draw_capture(i, j);

    i = x + 1; j = y - 1;
    while (is_valid_move(game, i, j)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i++; j--;
    }
    if (i < BOARD_SIZE && j >= 0 && can_capture(game, from_sq, j * 8 + i))
        draw_capture(i, j);
}

void show_queen_moves(t_game *game, int x, int y)
{
    show_rook_moves(game, x, y);
    show_bishop_moves(game, x, y);
}

void show_king_moves(t_game *game, int x, int y)
{
    Color lightBlack = (Color){22, 21, 17, 100};
    int from_sq = y * 8 + x;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            int tx = x + dx;
            int ty = y + dy;
            if (tx < 0 || tx >= BOARD_SIZE || ty < 0 || ty >= BOARD_SIZE) continue;
            if (is_valid_move(game, tx, ty))
                DrawCircle(tx * 100 + 50, ty * 100 + 50, 15, lightBlack);
            else if (can_capture(game, from_sq, ty * 8 + tx))
                draw_capture(tx, ty);
        }
    }
}

void show_knight_moves(t_game *game, int x, int y)
{
    Color lightBlack = (Color){22, 21, 17, 100};
    int from_sq = y * 8 + x;
    int moves[8][2] = {
        {x+2, y+1}, {x+2, y-1}, {x-2, y+1}, {x-2, y-1},
        {x+1, y+2}, {x+1, y-2}, {x-1, y+2}, {x-1, y-2}
    };
    for (int k = 0; k < 8; k++) {
        int tx = moves[k][0];
        int ty = moves[k][1];
        if (tx < 0 || tx >= BOARD_SIZE || ty < 0 || ty >= BOARD_SIZE) continue;
        if (is_valid_move(game, tx, ty))
            DrawCircle(tx * 100 + 50, ty * 100 + 50, 15, lightBlack);
        else if (can_capture(game, from_sq, ty * 8 + tx))
            draw_capture(tx, ty);
    }
}
