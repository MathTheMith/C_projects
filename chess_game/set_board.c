#include "chess_game.h"

Texture2D wR, bR, wN, bN, wB, bB, wK, bK, wQ, bQ, wP, bP;

bool is_piece_on_square(int board[BOARD_SIZE][BOARD_SIZE], int x, int y) {
    return board[x][y] != EMPTY;
}

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
        show_possibles_moves(game->board, game->old_x, game->old_y);
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++)
            if (is_piece_on_square(game->board, i, j))
                DrawTexture(get_texture(game->board[i][j]), i * 100, j * 100, WHITE);
}

static bool game_over(int board[BOARD_SIZE][BOARD_SIZE])
{
    bool wK_found = false, bK_found = false;
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == WK) wK_found = true;
            if (board[i][j] == BK) bK_found = true;
        }
    return !wK_found || !bK_found;
}

static void handle_input(t_game *game, int x, int y)
{
    if (game->is_piece == 0 && is_piece_on_square(game->board, x, y)) {
        bool is_white_piece = game->board[x][y] > 0;
        bool is_black_piece = game->board[x][y] < 0;
        if ((game->current_turn == 0 && is_white_piece) || (game->current_turn == 1 && is_black_piece)) {
            game->old_x = x;
            game->old_y = y;
            game->current_piece = game->board[x][y];
            game->is_piece = 1;
        }
    }
    else if (game->is_piece == 1) {
        if (is_valid_destination(game->board, game->old_x, game->old_y, x, y)) {
            game->board[x][y] = game->current_piece;
            game->board[game->old_x][game->old_y] = EMPTY;
            game->is_piece = 0;
            game->current_turn = 1 - game->current_turn;
        }
        else if (is_piece_on_square(game->board, x, y)) {
            bool is_white_piece = game->board[x][y] > 0;
            bool is_black_piece = game->board[x][y] < 0;
            if ((game->current_turn == 0 && is_white_piece) || (game->current_turn == 1 && is_black_piece)) {
                game->old_x = x;
                game->old_y = y;
                game->current_piece = game->board[x][y];
                game->is_piece = 1;
            }
        }
        else {
            game->is_piece = 0;
        }
    }
}

void set_board()
{
    t_game game = {0};
    SetTargetFPS(60);
    init_board(game.board);

    while (!WindowShouldClose())
    {
        Vector2 mousePos = GetMousePosition();
        int x = mousePos.x / 100;
        int y = mousePos.y / 100;
        Rectangle mouse_square = {x * 100, y * 100, 100, 100};

        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw_board(&game);
        if (CheckCollisionPointRec(mousePos, mouse_square) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            handle_input(&game, x, y);
            if (game_over(game.board)) break;
        }
        EndDrawing();
    }
    UnloadTextures();
}