#include "chess_game.h"

void init_board(int board[BOARD_SIZE][BOARD_SIZE])
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

    board[0][7] = WR; board[7][7] = WR;
    board[1][7] = WN; board[6][7] = WN;
    board[2][7] = WB; board[5][7] = WB;
    board[3][7] = WQ; board[4][7] = WK;

    board[0][0] = BR; board[7][0] = BR;
    board[1][0] = BN; board[6][0] = BN;
    board[2][0] = BB; board[5][0] = BB;
    board[3][0] = BQ; board[4][0] = BK;

    int i = 0;
    while (i < 8) {
        board[i][6] = WP;
        i++;
    }

    int j = 0;
    while (j < 8) {
        board[j][1] = BP;
        j++;
    }
}


Texture2D get_texture(int piece) {
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