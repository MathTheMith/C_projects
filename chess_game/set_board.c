#include "chess_game.h"

Texture2D wR, bR, wN, bN, wB, bB, wK, bK, wQ, bQ, wP, bP;

bool is_piece_on_square(Texture2D board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
    return board[x][y].id != 0;
}

void set_board(Texture2D board[BOARD_SIZE][BOARD_SIZE]) {
    Color lightBrown = (Color){181, 135, 99, 255};
    Color lightBeige = (Color){240, 218, 181, 255};
    Color lightBlack = (Color){22, 21, 17, 220};

    Texture2D current_piece = {0};

    int current_turn = 0;
    int is_piece = 0;
    int old_x;
    int old_y;

    SetTargetFPS(60);
    init_board(board);

    while (!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();
        int x = mousePos.x / 100;
        int y = mousePos.y / 100;
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int i = 0;
        while (i < BOARD_SIZE) {
            int j = 0;
            while (j < BOARD_SIZE) {
                Color color = ((i + j) % 2 == 0) ? lightBrown : lightBeige;
                    DrawRectangle(i * 100, j * 100, 100, 100, color);
                if (is_piece_on_square(board, i, j)) {
                    DrawTexture(board[i][j], i * 100, j * 100, WHITE);
                }
                j++;
            }
            i++;
        }
        if (is_piece == 1) {
            show_possibles_moves(board, old_x, old_y);
        }
        if (CheckCollisionPointRec(mousePos, (Rectangle){x * 100, y * 100, 100, 100}) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            
            if (is_piece == 0 && is_piece_on_square(board, x, y)) {
                bool is_white_piece = (board[x][y].id == wR.id || board[x][y].id == wN.id || 
                                       board[x][y].id == wB.id || board[x][y].id == wK.id || 
                                       board[x][y].id == wQ.id || board[x][y].id == wP.id);
                
                bool is_black_piece = (board[x][y].id == bR.id || board[x][y].id == bN.id || 
                                       board[x][y].id == bB.id || board[x][y].id == bK.id || 
                                       board[x][y].id == bQ.id || board[x][y].id == bP.id);
                if ((current_turn == 0 && is_white_piece) || (current_turn == 1 && is_black_piece)) {
                    old_x = x;
                    old_y = y;
                    current_piece = board[x][y];
                    is_piece = 1;
                }
            }
            else if (is_piece == 1) {
                if (is_valid_destination(board, old_x, old_y, x, y)) {
                    board[x][y] = current_piece;
                    board[old_x][old_y] = (Texture2D){0};
                    is_piece = 0;
                    current_turn = 1 - current_turn;
                }
                else if (is_piece_on_square(board, x, y)) {
                    bool is_white_piece = (board[x][y].id == wR.id || board[x][y].id == wN.id || 
                                           board[x][y].id == wB.id || board[x][y].id == wK.id || 
                                           board[x][y].id == wQ.id || board[x][y].id == wP.id);
                    
                    bool is_black_piece = (board[x][y].id == bR.id || board[x][y].id == bN.id || 
                                           board[x][y].id == bB.id || board[x][y].id == bK.id || 
                                           board[x][y].id == bQ.id || board[x][y].id == bP.id);
                    
                    if ((current_turn == 0 && is_white_piece) || (current_turn == 1 && is_black_piece)) {
                        old_x = x;
                        old_y = y;
                        current_piece = board[x][y];
                        is_piece = 1;
                    }
                }
                else {
                    is_piece = 0;
                }
            }
        }

        EndDrawing();
    }

    UnloadTexture(wP);
    UnloadTexture(bP);
    UnloadTexture(wR);
    UnloadTexture(bR);
    UnloadTexture(wN);
    UnloadTexture(bN);
    UnloadTexture(wB);
    UnloadTexture(bB);
    UnloadTexture(wK);
    UnloadTexture(bK);
    UnloadTexture(wQ);
    UnloadTexture(bQ);
}