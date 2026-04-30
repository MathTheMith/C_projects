#include "chess_game.h"
#include <stdlib.h>

bool is_valid_rook_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    if (old_x != new_x && old_y != new_y) return false;

    int step_x = (new_x > old_x) ? 1 : (new_x < old_x) ? -1 : 0;
    int step_y = (new_y > old_y) ? 1 : (new_y < old_y) ? -1 : 0;

    int x = old_x + step_x;
    int y = old_y + step_y;

    while (x != new_x || y != new_y)
    {
        if (board[x][y] != EMPTY) return false;
        x += step_x;
        y += step_y;
    }

    if (board[new_x][new_y] == EMPTY) return true;
    return can_capture(board, new_x, new_y, old_x, old_y);
}

bool is_valid_bishop_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    int dx = new_x - old_x;
    int dy = new_y - old_y;

    if (abs(dx) != abs(dy)) return false;

    int step_x = (dx > 0) ? 1 : -1;
    int step_y = (dy > 0) ? 1 : -1;

    int x = old_x + step_x;
    int y = old_y + step_y;

    while (x != new_x && y != new_y)
    {
        if (board[x][y] != EMPTY) return false;
        x += step_x;
        y += step_y;
    }

    if (board[new_x][new_y] == EMPTY) return true;
    return can_capture(board, new_x, new_y, old_x, old_y);
}

bool is_valid_queen_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    return is_valid_rook_move(board, old_x, old_y, new_x, new_y) ||
           is_valid_bishop_move(board, old_x, old_y, new_x, new_y);
}

bool is_valid_knight_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    int dx = abs(new_x - old_x);
    int dy = abs(new_y - old_y);

    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2))) return false;

    if (board[new_x][new_y] == EMPTY) return true;
    return can_capture(board, new_x, new_y, old_x, old_y);
}

bool is_valid_king_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    int dx = abs(new_x - old_x);
    int dy = abs(new_y - old_y);

    if (dx > 1 || dy > 1 || (dx == 0 && dy == 0)) return false;

    if (board[new_x][new_y] == EMPTY) return true;
    return can_capture(board, new_x, new_y, old_x, old_y);
}

bool is_valid_wpawn_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    if (new_x == old_x)
    {
        if (new_y == old_y - 1 && is_valid_move(new_x, new_y, board))
        {
            return true;
        }
        if (old_y == 6 && new_y == old_y - 2 &&
            is_valid_move(new_x, new_y, board) &&
            is_valid_move(new_x, old_y - 1, board))
        {
            return true;
        }
    }

    if ((new_x == old_x + 1 || new_x == old_x - 1) && new_y == old_y - 1)
    {
        if (board[new_x][new_y] < 0)
            return true;
    }

    return false;
}

bool is_valid_bpawn_move(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    if (new_x == old_x)
    {
        if (new_y == old_y + 1 && is_valid_move(new_x, new_y, board))
        {
            return true;
        }
        if (old_y == 1 && new_y == old_y + 2 &&
            is_valid_move(new_x, new_y, board) &&
            is_valid_move(new_x, old_y + 1, board))
        {
            return true;
        }
    }

    if ((new_x == old_x + 1 || new_x == old_x - 1) && new_y == old_y + 1)
    {
        if (board[new_x][new_y] > 0)
            return true;
    }

    return false;
}

bool is_valid_destination(int board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y)
{
    if (old_x == new_x && old_y == new_y) return false;

    if (board[old_x][old_y] == WR || board[old_x][old_y] == BR)
        return is_valid_rook_move(board, old_x, old_y, new_x, new_y);

    if (board[old_x][old_y] == WB || board[old_x][old_y] == BB)
        return is_valid_bishop_move(board, old_x, old_y, new_x, new_y);

    if (board[old_x][old_y] == WN || board[old_x][old_y] == BN)
        return is_valid_knight_move(board, old_x, old_y, new_x, new_y);

    if (board[old_x][old_y] == WQ || board[old_x][old_y] == BQ)
        return is_valid_queen_move(board, old_x, old_y, new_x, new_y);

    if (board[old_x][old_y] == WK || board[old_x][old_y] == BK)
        return is_valid_king_move(board, old_x, old_y, new_x, new_y);

    if (board[old_x][old_y] == WP)
        return is_valid_wpawn_move(board, old_x, old_y, new_x, new_y);

    if (board[old_x][old_y] == BP)
        return is_valid_bpawn_move(board, old_x, old_y, new_x, new_y);

    return false;
}

bool can_capture(int board[BOARD_SIZE][BOARD_SIZE], int x, int y, int old_x, int old_y)
{
    if (board[old_x][old_y] > 0 && board[x][y] < 0) return true;
    if (board[old_x][old_y] < 0 && board[x][y] > 0) return true;
    return false;
}
