#include "raylib.h"
#include "chess_game.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Chess game");
    SetTraceLogLevel(LOG_NONE);
    
    set_board();
    
    CloseWindow();
    return 0;
}