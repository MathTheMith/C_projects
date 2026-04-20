#include "tetris.h"
#include "game.h"
#include "pieces.h"
#include "utils.h"

int main(void)
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris");
    SetTargetFPS(120);
    SetWindowPosition(0, 0);
    
    GameState *game = init_game();
    
    // Countdown au démarrage
    int start_delay = 360; // 3 secondes
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        draw_grid();

        if (start_delay > 0)
        {
            int seconds = (start_delay / 120) + 1;
            DrawText(TextFormat("%d", seconds), 420, 450, 80, WHITE);
            start_delay--;
            draw_map(game->map);
            draw_piece(&game->current);
            EndDrawing();
            continue;
        }

        if (check_game_over(game))
        {
            draw_map(game->map);
            draw_game_over_screen(game->score);
            EndDrawing();
            if (IsKeyPressed(KEY_R))
            {
                free_game(game);
                game = init_game();
                start_delay = 360;
            }
            continue;
        }

        update_game(game);
        EndDrawing();
    }
    
    free_game(game);
    CloseWindow();
    return 0;
}