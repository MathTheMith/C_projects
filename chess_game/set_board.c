#include "chess_game.h"
#include "engine/engine.h"

#define MAX_HISTORY 512

typedef struct {
    t_game state;
} t_history_entry;

void draw_board(t_game *game)
{
    Color lightBrown = (Color){181, 135, 99, 255};
    Color lightBeige = (Color){240, 218, 181, 255};

    Color yellowDark  = (Color){170, 162,  58, 255};
    Color yellowLight = (Color){205, 210, 106, 255};

    for (int i = 0; i < BOARD_SIZE; i++)
        for (int j = 0; j < BOARD_SIZE; j++) {
            int sq = j * 8 + i;
            bool is_last = (sq == game->last_from || sq == game->last_to);
            bool is_dark = (i + j) % 2 == 0;
            Color color;
            if (is_last)
                color = is_dark ? yellowDark : yellowLight;
            else
                color = is_dark ? lightBrown : lightBeige;
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
    if (game->wp.king == 0 || game->bp.king == 0) return true;
    return !has_legal_moves(game);
}

static void push_state(t_game *game, t_history_entry *hist, int *hist_current, int *hist_count)
{
    *hist_count = *hist_current + 1;
    if (*hist_count < MAX_HISTORY)
        hist[(*hist_count)++].state = *game;
    *hist_current = *hist_count - 1;
}

static void apply_robot_move(t_game *game, t_history_entry *hist, int *hist_current, int *hist_count)
{
    int saved_turn = game->current_turn;
    t_move best = generate_moves(game);
    game->current_turn = saved_turn;

    if (best.from_x == best.to_x && best.from_y == best.to_y) return;
    if (get_piece(game, best.from_y * 8 + best.from_x) == EMPTY) return;

    game->old_x = best.from_x;
    game->old_y = best.from_y;
    move_pieces(game, best.to_x, best.to_y);
    int moved = get_piece(game, best.to_y * 8 + best.to_x);
    if (moved == BP && best.to_y == 7)
    {
        int promo_sq = best.to_y * 8 + best.to_x;
        game->bp.pawns &= ~(1ULL << promo_sq);
        game->bp.queen |=  (1ULL << promo_sq);
        game->hash ^= zobrist_table[piece_to_index(BP)][promo_sq];
        game->hash ^= zobrist_table[piece_to_index(BQ)][promo_sq];
    }
    game->current_turn = 1 - game->current_turn;
    game->last_from = best.from_y * 8 + best.from_x;
    game->last_to   = best.to_y   * 8 + best.to_x;
    push_state(game, hist, hist_current, hist_count);
}

static void handle_input(t_game *game, int x, int y, t_history_entry *hist, int *hist_current, int *hist_count)
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
        if (is_valid_destination(game, game->old_x, game->old_y, x, y))
        {
            int from_sq = game->old_y * 8 + game->old_x;
            move_pieces(game, x, y);
            if ((game->current_piece == WP && y == 0) || (game->current_piece == BP && y == 7))
            {
                int promo_sq = y * 8 + x;
                game->wp.pawns &= ~(1ULL << promo_sq);
                game->wp.queen |=  (1ULL << promo_sq);
                game->hash ^= zobrist_table[piece_to_index(WP)][promo_sq];
                game->hash ^= zobrist_table[piece_to_index(WQ)][promo_sq];
            }
            game->is_piece = 0;
            game->current_turn = 1 - game->current_turn;
            game->last_from = from_sq;
            game->last_to   = y * 8 + x;
            push_state(game, hist, hist_current, hist_count);
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
    t_history_entry hist[MAX_HISTORY];
    int hist_current = 0;
    int hist_count   = 1;

    SetTargetFPS(60);
    init_board();
    init_zobrist();
    set_pieces(&game.wp, &game.bp);
    game.ep_square = -1;
    game.castling  = 0x0F;
    game.robot_color = 1;
    game.last_from = -1;
    game.last_to   = -1;
    game.hash = compute_hash(&game);
    hist[0].state = game;

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

        if (IsKeyPressed(KEY_LEFT) && hist_current >= 2)
        {
            hist_current -= 2;
            game = hist[hist_current].state;
        }
        else if (IsKeyPressed(KEY_RIGHT) && hist_current + 2 < hist_count)
        {
            hist_current += 2;
            game = hist[hist_current].state;
        }

        if (game.current_turn == game.robot_color) {
            if (game_over(&game)) break;
            apply_robot_move(&game, hist, &hist_current, &hist_count);
            if (game_over(&game)) break;
        } else if (CheckCollisionPointRec(mousePos, mouse_square) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)
            && x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
            handle_input(&game, x, y, hist, &hist_current, &hist_count);
            if (game_over(&game)) break;
        }
    }
    UnloadTextures();
}
