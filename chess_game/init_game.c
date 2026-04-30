#include "chess_game.h"

Texture2D wR, bR, wN, bN, wB, bB, wK, bK, wQ, bQ, wP, bP;

void init_board()
{
    wP = LoadTexture("./Images/wP.png");
    bP = LoadTexture("./Images/bP.png");
    wR = LoadTexture("./Images/wR.png");
    bR = LoadTexture("./Images/bR.png");
    wN = LoadTexture("./Images/wN.png");
    bN = LoadTexture("./Images/bN.png");
    wB = LoadTexture("./Images/wB.png");
    bB = LoadTexture("./Images/bB.png");
    wK = LoadTexture("./Images/wK.png");
    bK = LoadTexture("./Images/bK.png");
    wQ = LoadTexture("./Images/wQ.png");
    bQ = LoadTexture("./Images/bQ.png");
}

int get_piece(t_game *game, int sq)
{
    uint64_t mask = 1ULL << sq;
    if (game->wp.pawns  & mask) return WP;
    if (game->wp.rooks  & mask) return WR;
    if (game->wp.knight & mask) return WN;
    if (game->wp.bishop & mask) return WB;
    if (game->wp.queen  & mask) return WQ;
    if (game->wp.king   & mask) return WK;
    if (game->bp.pawns  & mask) return BP;
    if (game->bp.rooks  & mask) return BR;
    if (game->bp.knight & mask) return BN;
    if (game->bp.bishop & mask) return BB;
    if (game->bp.queen  & mask) return BQ;
    if (game->bp.king   & mask) return BK;
    return EMPTY;
}

Texture2D get_texture(int piece)
{
    switch (piece) {
        case WP: return wP;
        case WR: return wR;
        case WN: return wN;
        case WB: return wB;
        case WQ: return wQ;
        case WK: return wK;
        case BP: return bP;
        case BR: return bR;
        case BN: return bN;
        case BB: return bB;
        case BQ: return bQ;
        case BK: return bK;
        default: return (Texture2D){0};
    }
}

void UnloadTextures()
{
    UnloadTexture(wP); UnloadTexture(bP);
    UnloadTexture(wR); UnloadTexture(bR);
    UnloadTexture(wN); UnloadTexture(bN);
    UnloadTexture(wB); UnloadTexture(bB);
    UnloadTexture(wK); UnloadTexture(bK);
    UnloadTexture(wQ); UnloadTexture(bQ);
}
