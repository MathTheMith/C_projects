#include "raylib.h"
#include "chess_game.h"

bool is_valid_move(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y].id == 0;
}

static void draw_capture(int x, int y) {
    Color captureGrey = (Color){80, 80, 80, 75};
    DrawRectangle(x * 100, y * 100, 100, 100, captureGrey);
}

void show_possibles_moves(Texture2D board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
    if (board[x][y].id == wR.id || board[x][y].id == bR.id) {
        show_rook_moves(x, y, board);
    }
    if (board[x][y].id == wB.id || board[x][y].id == bB.id) {
        show_bishop_moves(x, y, board);
    }
    if (board[x][y].id == wN.id || board[x][y].id == bN.id) {
        show_knight_moves(x, y, board);
    }
    if (board[x][y].id == wQ.id || board[x][y].id == bQ.id) {
        show_queen_moves(x, y, board);
    }
    if (board[x][y].id == wK.id || board[x][y].id == bK.id) {
        show_king_moves(x, y, board);
    }
    if (board[x][y].id == wP.id) {
        show_wpawns_moves(x, y, board);
    }
    if (board[x][y].id == bP.id) {
        show_bpawns_moves(x, y, board);
    }
}

void show_wpawns_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    int i;
    Color lightBlack = (Color){22, 21, 17, 100};
    i = y - 1;
    if (y == 6 && is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i--;
        if (is_valid_move(x, i, board)) {
            DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        }
    }
    else if (is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
    }
    // Captures diagonales
    if (x + 1 < BOARD_SIZE && y - 1 >= 0 && can_capture(board, x + 1, y - 1, x, y))
        draw_capture(x + 1, y - 1);
    if (x - 1 >= 0 && y - 1 >= 0 && can_capture(board, x - 1, y - 1, x, y))
        draw_capture(x - 1, y - 1);
}

void show_bpawns_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    int i;
    Color lightBlack = (Color){22, 21, 17, 100};
    i = y + 1;
    if (y == 1 && is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i++;
        if (is_valid_move(x, i, board)) {
            DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        }
    }
    else if (is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
    }
    // Captures diagonales
    if (x + 1 < BOARD_SIZE && y + 1 < BOARD_SIZE && can_capture(board, x + 1, y + 1, x, y))
        draw_capture(x + 1, y + 1);
    if (x - 1 >= 0 && y + 1 < BOARD_SIZE && can_capture(board, x - 1, y + 1, x, y))
        draw_capture(x - 1, y + 1);
}

void show_rook_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    int i;
    Color lightBlack = (Color){22, 21, 17, 100};
    i = x + 1;
    while (i < BOARD_SIZE && is_valid_move(i, y, board)) {
        DrawCircle(i * 100 + 50, y * 100 + 50, 15, lightBlack);
        i++;
    }
    if (i < BOARD_SIZE && can_capture(board, i, y, x, y))
        draw_capture(i, y);

    i = x - 1;
    while (i >= 0 && is_valid_move(i, y, board)) {
        DrawCircle(i * 100 + 50, y * 100 + 50, 15, lightBlack);
        i--;
    }
    if (i >= 0 && can_capture(board, i, y, x, y))
        draw_capture(i, y);

    i = y + 1;
    while (i < BOARD_SIZE && is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i++;
    }
    if (i < BOARD_SIZE && can_capture(board, x, i, x, y))
        draw_capture(x, i);

    i = y - 1;
    while (i >= 0 && is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i--;
    }
    if (i >= 0 && can_capture(board, x, i, x, y))
        draw_capture(x, i);
}

void show_bishop_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    int i = x + 1, j = y + 1;
    Color lightBlack = (Color){22, 21, 17, 100};
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i++;
        j++;
    }
    if (i < BOARD_SIZE && j < BOARD_SIZE && can_capture(board, i, j, x, y))
        draw_capture(i, j);

    i = x - 1;
    j = y - 1;
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i--;
        j--;
    }
    if (i >= 0 && j >= 0 && can_capture(board, i, j, x, y))
        draw_capture(i, j);

    i = x - 1;
    j = y + 1;
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i--;
        j++;
    }
    if (i >= 0 && j < BOARD_SIZE && can_capture(board, i, j, x, y))
        draw_capture(i, j);

    i = x + 1;
    j = y - 1;
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i++;
        j--;
    }
    if (i < BOARD_SIZE && j >= 0 && can_capture(board, i, j, x, y))
        draw_capture(i, j);
}

void show_queen_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    show_rook_moves(x, y, board);
    show_bishop_moves(x, y, board);
}

void show_king_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    Color lightBlack = (Color){22, 21, 17, 100};
    int dx, dy, tx, ty;
    for (dx = -1; dx <= 1; dx++) {
        for (dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            tx = x + dx;
            ty = y + dy;
            if (tx < 0 || tx >= BOARD_SIZE || ty < 0 || ty >= BOARD_SIZE) continue;
            if (is_valid_move(tx, ty, board))
                DrawCircle(tx * 100 + 50, ty * 100 + 50, 15, lightBlack);
            else if (can_capture(board, tx, ty, x, y))
                draw_capture(tx, ty);
        }
    }
}

void show_knight_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    Color lightBlack = (Color){22, 21, 17, 100};
    int moves[8][2] = {
        {x+2, y+1}, {x+2, y-1}, {x-2, y+1}, {x-2, y-1},
        {x+1, y+2}, {x+1, y-2}, {x-1, y+2}, {x-1, y-2}
    };
    int k, tx, ty;
    for (k = 0; k < 8; k++) {
        tx = moves[k][0];
        ty = moves[k][1];
        if (tx < 0 || tx >= BOARD_SIZE || ty < 0 || ty >= BOARD_SIZE) continue;
        if (is_valid_move(tx, ty, board))
            DrawCircle(tx * 100 + 50, ty * 100 + 50, 15, lightBlack);
        else if (can_capture(board, tx, ty, x, y))
            draw_capture(tx, ty);
    }
}
