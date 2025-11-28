#include "raylib.h"
#include "chess_game.h"

bool is_valid_move(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y].id == 0;
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
}

void show_rook_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    int i;
    Color lightBlack = (Color){22, 21, 17, 100};
    i = x + 1;
    while (i < BOARD_SIZE && is_valid_move(i, y, board)) {
        DrawCircle(i * 100 + 50, y * 100 + 50, 15, lightBlack);
        i++;
    }
    
    i = x - 1;
    while (i >= 0 && is_valid_move(i, y, board)) {
        DrawCircle(i * 100 + 50, y * 100 + 50, 15, lightBlack);
        i--;
    }
    
    i = y + 1;
    while (i < BOARD_SIZE && is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i++;
    }
    
    i = y - 1;
    while (i >= 0 && is_valid_move(x, i, board)) {
        DrawCircle(x * 100 + 50, i * 100 + 50, 15, lightBlack);
        i--;
    }
}

void show_bishop_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    int i = x + 1, j = y + 1;
    Color lightBlack = (Color){22, 21, 17, 100};
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i++;
        j++;
    }
    i = x - 1;
    j = y - 1;
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i--;
        j--;
    }
    i = x - 1;
    j = y + 1;
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i--;
        j++;
    }
    i = x + 1;
    j = y - 1;
    while (is_valid_move(i, j, board)) {
        DrawCircle(i * 100 + 50, j * 100 + 50, 15, lightBlack);
        i++;
        j--;
    }
}

void show_queen_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    show_rook_moves(x, y, board);
    show_bishop_moves(x, y, board);
}

void show_king_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    Color lightBlack = (Color){22, 21, 17, 100};

    if (is_valid_move(x + 1, y, board)) {
        DrawCircle((x + 1) * 100 + 50, y * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x - 1, y, board)) {
        DrawCircle((x - 1) * 100 + 50, y * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x, y + 1, board)) {
        DrawCircle(x * 100 + 50, (y + 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x, y - 1, board)) {
        DrawCircle(x * 100 + 50, (y - 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x + 1, y + 1, board)) {
        DrawCircle((x + 1) * 100 + 50, (y + 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x + 1, y - 1, board)) {
        DrawCircle((x + 1) * 100 + 50, (y - 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x - 1, y + 1, board)) {
        DrawCircle((x - 1) * 100 + 50, (y + 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x - 1, y - 1, board)) {
        DrawCircle((x - 1) * 100 + 50, (y - 1) * 100 + 50, 15, lightBlack);
    }
}

void show_knight_moves(int x, int y, Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    Color lightBlack = (Color){22, 21, 17, 100};
    if (is_valid_move(x + 2, y + 1, board)) {
        DrawCircle((x + 2) * 100 + 50, (y + 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x + 2, y - 1, board)) {
        DrawCircle((x + 2) * 100 + 50, (y - 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x - 2, y + 1, board)) {
        DrawCircle((x - 2) * 100 + 50, (y + 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x - 2, y - 1, board)) {
        DrawCircle((x - 2) * 100 + 50, (y - 1) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x + 1, y + 2, board)) {
        DrawCircle((x + 1) * 100 + 50, (y + 2) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x + 1, y - 2, board)) {
        DrawCircle((x + 1) * 100 + 50, (y - 2) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x - 1, y + 2, board)) {
        DrawCircle((x - 1) * 100 + 50, (y + 2) * 100 + 50, 15, lightBlack);
    }
    if (is_valid_move(x - 1, y - 2, board)) {
        DrawCircle((x - 1) * 100 + 50, (y - 2) * 100 + 50, 15, lightBlack);
    }
}
