#include "chess_game.h"

#include "chess_game.h"

bool is_valid_rook_move(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    // Mouvement horizontal
    if (old_y == new_y) {
        int step = (new_x > old_x) ? 1 : -1;
        int i = old_x + step;
        while (i != new_x) {
            if (!is_valid_move(i, old_y, board)) return false;
            i += step;
        }
        return is_valid_move(new_x, new_y, board);
    }
    // Mouvement vertical
    if (old_x == new_x) {
        int step = (new_y > old_y) ? 1 : -1;
        int j = old_y + step;
        while (j != new_y) {
            if (!is_valid_move(old_x, j, board)) return false;
            j += step;
        }
        return is_valid_move(new_x, new_y, board);
    }
    return false;
}

bool is_valid_bishop_move(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    int dx = new_x - old_x;
    int dy = new_y - old_y;
    
    // Vérifier si c'est un mouvement diagonal
    if (dx * dx != dy * dy) return false;
    
    int step_x = (dx > 0) ? 1 : -1;
    int step_y = (dy > 0) ? 1 : -1;
    
    int i = old_x + step_x;
    int j = old_y + step_y;
    
    while (i != new_x && j != new_y) {
        if (!is_valid_move(i, j, board)) return false;
        i += step_x;
        j += step_y;
    }
    
    return is_valid_move(new_x, new_y, board);
}

bool is_valid_knight_move(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    int dx = new_x - old_x;
    int dy = new_y - old_y;
    
    // Vérifier les 8 positions possibles du cavalier
    if ((dx == 2 && dy == 1) || (dx == 2 && dy == -1) ||
        (dx == -2 && dy == 1) || (dx == -2 && dy == -1) ||
        (dx == 1 && dy == 2) || (dx == 1 && dy == -2) ||
        (dx == -1 && dy == 2) || (dx == -1 && dy == -2)) {
        return is_valid_move(new_x, new_y, board);
    }
    
    return false;
}

bool is_valid_queen_move(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    // La reine peut bouger comme une tour ou un fou
    return is_valid_rook_move(board, old_x, old_y, new_x, new_y) || 
           is_valid_bishop_move(board, old_x, old_y, new_x, new_y);
}

bool is_valid_king_move(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    int dx = new_x - old_x;
    int dy = new_y - old_y;
    
    // Le roi peut bouger d'une case dans toutes les directions
    if (dx >= -1 && dx <= 1 && dy >= -1 && dy <= 1 && !(dx == 0 && dy == 0)) {
        return is_valid_move(new_x, new_y, board);
    }
    
    return false;
}

bool is_valid_wpawn_move(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    // Mouvement vers le haut (y diminue)
    if (new_x == old_x) {
        // Un pas en avant
        if (new_y == old_y - 1 && is_valid_move(new_x, new_y, board)) {
            return true;
        }
        // Deux pas depuis la position initiale
        if (old_y == 6 && new_y == old_y - 2 && 
            is_valid_move(new_x, new_y, board) && 
            is_valid_move(new_x, old_y - 1, board)) {
            return true;
        }
    }
    
    // Capture en diagonale
    if ((new_x == old_x + 1 || new_x == old_x - 1) && new_y == old_y - 1) {
        if (board[new_x][new_y].id != 0) {
            // Vérifier que c'est une pièce noire
            if (board[new_x][new_y].id == bR.id || board[new_x][new_y].id == bN.id ||
                board[new_x][new_y].id == bB.id || board[new_x][new_y].id == bK.id ||
                board[new_x][new_y].id == bQ.id || board[new_x][new_y].id == bP.id) {
                return true;
            }
        }
    }
    
    return false;
}

bool is_valid_bpawn_move(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    // Mouvement vers le bas (y augmente)
    if (new_x == old_x) {
        // Un pas en avant
        if (new_y == old_y + 1 && is_valid_move(new_x, new_y, board)) {
            return true;
        }
        // Deux pas depuis la position initiale
        if (old_y == 1 && new_y == old_y + 2 && 
            is_valid_move(new_x, new_y, board) && 
            is_valid_move(new_x, old_y + 1, board)) {
            return true;
        }
    }
    
    // Capture en diagonale
    if ((new_x == old_x + 1 || new_x == old_x - 1) && new_y == old_y + 1) {
        if (board[new_x][new_y].id != 0) {
            // Vérifier que c'est une pièce blanche
            if (board[new_x][new_y].id == wR.id || board[new_x][new_y].id == wN.id ||
                board[new_x][new_y].id == wB.id || board[new_x][new_y].id == wK.id ||
                board[new_x][new_y].id == wQ.id || board[new_x][new_y].id == wP.id) {
                return true;
            }
        }
    }
    
    return false;
}

bool is_valid_destination(Texture2D board[BOARD_SIZE][BOARD_SIZE], int old_x, int old_y, int new_x, int new_y) {
    if (board[old_x][old_y].id == wR.id || board[old_x][old_y].id == bR.id) {
        return is_valid_rook_move(board, old_x, old_y, new_x, new_y);
    }
    if (board[old_x][old_y].id == wB.id || board[old_x][old_y].id == bB.id) {
        return is_valid_bishop_move(board, old_x, old_y, new_x, new_y);
    }
    if (board[old_x][old_y].id == wN.id || board[old_x][old_y].id == bN.id) {
        return is_valid_knight_move(board, old_x, old_y, new_x, new_y);
    }
    if (board[old_x][old_y].id == wQ.id || board[old_x][old_y].id == bQ.id) {
        return is_valid_queen_move(board, old_x, old_y, new_x, new_y);
    }
    if (board[old_x][old_y].id == wK.id || board[old_x][old_y].id == bK.id) {
        return is_valid_king_move(board, old_x, old_y, new_x, new_y);
    }
    if (board[old_x][old_y].id == wP.id) {
        return is_valid_wpawn_move(board, old_x, old_y, new_x, new_y);
    }
    if (board[old_x][old_y].id == bP.id) {
        return is_valid_bpawn_move(board, old_x, old_y, new_x, new_y);
    }
    
    return false;
}

bool can_capture(Texture2D board[BOARD_SIZE][BOARD_SIZE], int x, int y, int old_x, int old_y)
{
    bool is_selected_white = (board[old_x][old_y].id == wR.id || 
                            board[old_x][old_y].id == wN.id || 
                            board[old_x][old_y].id == wB.id || 
                            board[old_x][old_y].id == wK.id || 
                            board[old_x][old_y].id == wQ.id ||
                            board[old_x][old_y].id == wP.id);

    bool is_target_black = (board[x][y].id == bR.id || 
                          board[x][y].id == bN.id || 
                          board[x][y].id == bB.id || 
                          board[x][y].id == bK.id || 
                          board[x][y].id == bQ.id ||
                          board[x][y].id == bP.id);

    bool is_selected_black = (board[old_x][old_y].id == bR.id || 
                            board[old_x][old_y].id == bN.id || 
                            board[old_x][old_y].id == bB.id || 
                            board[old_x][old_y].id == bK.id || 
                            board[old_x][old_y].id == bQ.id ||
                            board[old_x][old_y].id == bP.id);

    bool is_target_white = (board[x][y].id == wR.id || 
                          board[x][y].id == wN.id || 
                          board[x][y].id == wB.id || 
                          board[x][y].id == wK.id || 
                          board[x][y].id == wQ.id ||
                          board[x][y].id == wP.id);

    if (is_selected_white && is_target_black)
        return true;

    if (is_selected_black && is_target_white)
        return true;

    return false;
}
