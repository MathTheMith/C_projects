#include "chess_game.h"
#include "engine/engine.h"

void draw_board(t_game *game)
{
    Color lightBrown = (Color){181, 135, 99, 255};
    Color lightBeige = (Color){240, 218, 181, 255};

    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++) {
            Color color = ((i + j) % 2 == 0) ? lightBrown : lightBeige;
            DrawRectangle(i * 100, j * 100, 100, 100, color);
        }

    if (game->is_piece)
        show_possibles_moves(game, game->old_x, game->old_y);

    uint64_t occ = get_occupied_bb(game);
    while (occ) {
        int sq = __builtin_ctzll(occ);
        int x  = sq % 8;
        int y  = sq / 8;
        DrawTexture(get_texture(get_piece(game, sq)), x * 100, y * 100, WHITE);
        occ &= occ - 1;
    }
}

bool game_over(t_game *game)
{
    return game->wp.king == 0 || game->bp.king == 0;
}

static void apply_robot_move(t_game *game)
{
    t_move best = generate_moves(game);
    game->old_x = best.from_x;
    game->old_y = best.from_y;
    move_pieces(game, best.to_x, best.to_y);
    game->current_turn = 1 - game->current_turn;
}

static void handle_input(t_game *game, int x, int y)
{
    uint64_t wp   = get_white_bb(game);
    uint64_t bp   = get_black_bb(game);
    uint64_t mask = 1ULL << (y * 8 + x);

    if (game->is_piece == 0) {
        if (!((wp | bp) & mask)) return;
        bool is_white = (wp & mask) != 0;
        bool is_black = (bp & mask) != 0;
        if ((game->current_turn == 0 && is_white) || (game->current_turn == 1 && is_black)) {
            game->old_x = x;
            game->old_y = y;
            game->current_piece = get_piece(game, y * 8 + x);
            game->is_piece = 1;
        }
    } else {
        if (is_valid_destination(game, game->old_x, game->old_y, x, y)) {
            move_pieces(game, x, y);
            game->is_piece = 0;
            game->current_turn = 1 - game->current_turn;
        } else if ((wp | bp) & mask) {
            bool is_white = (wp & mask) != 0;
            bool is_black = (bp & mask) != 0;
            if ((game->current_turn == 0 && is_white) || (game->current_turn == 1 && is_black)) {
                game->old_x = x;
                game->old_y = y;
                game->current_piece = get_piece(game, y * 8 + x);
                game->is_piece = 1;
            } else {
                game->is_piece = 0;
            }
        } else {
            game->is_piece = 0;
        }
    }
}

void set_board()
{
    t_game game = {0};
    SetTargetFPS(60);
    init_board();
    set_pieces(&game.wp, &game.bp);
    game.robot_color = 1;
    while (!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();
        int x = (int)mousePos.x / 100;
        int y = (int)mousePos.y / 100;
        Rectangle mouse_square = {x * 100, y * 100, 100, 100};

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_board(&game);
        EndDrawing();

        if (game.current_turn == game.robot_color) {
            apply_robot_move(&game);
            if (game_over(&game)) break;
        } else if (CheckCollisionPointRec(mousePos, mouse_square) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
            && x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
            handle_input(&game, x, y);
            if (game_over(&game)) break;
        }
    }
    UnloadTextures();
}
