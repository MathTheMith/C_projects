#include "raylib.h"
#include "chess_game.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Jeu d'échecs");
    SetTraceLogLevel(LOG_NONE);
    
    Texture2D board[BOARD_SIZE][BOARD_SIZE] = {0};
    
    set_board(board);
    
    CloseWindow();
    return 0;
}