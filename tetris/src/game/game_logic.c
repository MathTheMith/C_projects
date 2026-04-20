#include "tetris.h"
#include "game.h"
#include "pieces.h"
#include "utils.h"

static int next_queue[5];

GameState* init_game(void)
{
    GameState *state = malloc(sizeof(GameState));
    
    state->map = init_map();
    state->held_piece = -1;
    state->can_hold = 1;
    state->frame_counter = 0;
    state->score = 0;
    state->gamepad_id = 0;
    state->game_over = 0;
    
    // Détection manette
    for (int i = 0; i < 4; i++)
    {
        if (IsGamepadAvailable(i))
        {
            printf("Gamepad %d: %s\n", i, GetGamepadName(i));
            state->gamepad_id = i;
            break;
        }
    }
    
    // Initialise le système de pièces
    srand(time(NULL));
    init_bag_system();
    get_next_queue(next_queue, 5);
    
    // Première pièce
    state->current.type = get_next_piece();
    state->current.x = 395;
    state->current.y = 150;
    state->current.rotation = 0;
    
    return state;
}

void free_game(GameState *state)
{
    for (int i = 0; i < MAP_HEIGHT; i++)
        free(state->map[i]);
    free(state->map);
    free(state);
}

void update_game(GameState *state)
{
    handle_input(state);

    get_next_queue(next_queue, 5);
    draw_map(state->map);
    draw_ui(state, next_queue);
    draw_ghost_piece(state->map, &state->current);
    draw_piece(&state->current);

    state->frame_counter++;
}

int check_game_over(GameState *state)
{
    if (state->game_over)
        return 1;
    for (int j = 0; j < MAP_WIDTH; j++)
    {
        if (state->map[0][j] != 0)
            return 1;
    }
    return 0;
}

void draw_game_over_screen(int score)
{
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, (Color){0, 0, 0, 180});
    DrawText("GAME OVER", 210, 380, 80, RED);
    DrawText(TextFormat("Score final : %d", score), 280, 480, 35, WHITE);
    DrawText("R  - Recommencer", 295, 560, 28, LIGHTGRAY);
    DrawText("ESC - Quitter", 320, 600, 28, LIGHTGRAY);
}