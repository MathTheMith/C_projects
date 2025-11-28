#include "chess_game.h"

void init_board(Texture2D board[BOARD_SIZE][BOARD_SIZE])
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
    
    board[0][7] = wR; board[7][7] = wR;
    board[1][7] = wN; board[6][7] = wN;
    board[2][7] = wB; board[5][7] = wB;
    board[3][7] = wQ; board[4][7] = wK;

    board[0][0] = bR; board[7][0] = bR;
    board[1][0] = bN; board[6][0] = bN;
    board[2][0] = bB; board[5][0] = bB;
    board[3][0] = bQ; board[4][0] = bK;

    int i = 0;
    while (i < 8) {
        board[i][6] = wP;
        i++;
    }
    
    int j = 0;
    while (j < 8) {
        board[j][1] = bP;
        j++;
    }
}