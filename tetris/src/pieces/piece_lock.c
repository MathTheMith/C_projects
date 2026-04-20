#include "pieces.h"

int lock_piece(char **map, Piece *piece)
{
    int colonne = (piece->x - 215) / SIZE_SQUARE;
    int ligne = (piece->y - 100) / SIZE_SQUARE;

    if (!is_position_valid(map, piece))
        return -1;
    if (piece->type == 0) // I
    {
        if (piece->rotation == 0 || piece->rotation == 2)
        {
            map[ligne][colonne - 2] = 2;
            map[ligne][colonne - 1] = 2;
            map[ligne][colonne] = 2;
            map[ligne][colonne + 1] = 2;
        }
        else
        {
            map[ligne - 2][colonne] = 2;
            map[ligne - 1][colonne] = 2;
            map[ligne][colonne] = 2;
            map[ligne + 1][colonne] = 2;
        }
        return 0;
    }
    
    if (piece->type == 1) // O
    {
        map[ligne - 1][colonne - 1] = 1;
        map[ligne - 1][colonne] = 1;
        map[ligne][colonne - 1] = 1;
        map[ligne][colonne] = 1;
        return 0;
    }
    
    if (piece->type == 2) // T
    {
        switch (piece->rotation)
        {
            case 0:
                map[ligne][colonne - 1] = 3;
                map[ligne - 1][colonne] = 3;
                map[ligne][colonne] = 3;
                map[ligne][colonne + 1] = 3;
                break;
            case 1:
                map[ligne - 1][colonne] = 3;
                map[ligne][colonne] = 3;
                map[ligne][colonne + 1] = 3;
                map[ligne + 1][colonne] = 3;
                break;
            case 2:
                map[ligne][colonne - 1] = 3;
                map[ligne][colonne] = 3;
                map[ligne + 1][colonne] = 3;
                map[ligne][colonne + 1] = 3;
                break;
            case 3:
                map[ligne - 1][colonne] = 3;
                map[ligne][colonne - 1] = 3;
                map[ligne][colonne] = 3;
                map[ligne + 1][colonne] = 3;
                break;
        }
        return 0;
    }
    
    if (piece->type == 3) // S
    {
        if (piece->rotation == 0 || piece->rotation == 2)
        {
            map[ligne][colonne - 1] = 4;
            map[ligne - 1][colonne] = 4;
            map[ligne][colonne] = 4;
            map[ligne - 1][colonne + 1] = 4;
        }
        else
        {
            map[ligne - 1][colonne] = 4;
            map[ligne][colonne] = 4;
            map[ligne][colonne + 1] = 4;
            map[ligne + 1][colonne + 1] = 4;
        }
        return 0;
    }
    
    if (piece->type == 4) // Z
    {
        if (piece->rotation == 0 || piece->rotation == 2)
        {
            map[ligne - 1][colonne - 1] = 5;
            map[ligne - 1][colonne] = 5;
            map[ligne][colonne] = 5;
            map[ligne][colonne + 1] = 5;
        }
        else
        {
            map[ligne - 1][colonne + 1] = 5;
            map[ligne][colonne] = 5;
            map[ligne][colonne + 1] = 5;
            map[ligne + 1][colonne] = 5;
        }
        return 0;
    }
    
    if (piece->type == 5) // L
    {
        switch (piece->rotation)
        {
            case 0:
                map[ligne][colonne - 1] = 6;
                map[ligne][colonne] = 6;
                map[ligne][colonne + 1] = 6;
                map[ligne - 1][colonne + 1] = 6;
                break;
            case 1:
                map[ligne - 1][colonne] = 6;
                map[ligne][colonne] = 6;
                map[ligne + 1][colonne] = 6;
                map[ligne + 1][colonne + 1] = 6;
                break;
            case 2:
                map[ligne][colonne - 1] = 6;
                map[ligne][colonne] = 6;
                map[ligne][colonne + 1] = 6;
                map[ligne + 1][colonne - 1] = 6;
                break;
            case 3:
                map[ligne - 1][colonne - 1] = 6;
                map[ligne - 1][colonne] = 6;
                map[ligne][colonne] = 6;
                map[ligne + 1][colonne] = 6;
                break;
        }
        return 0;
    }
    
    if (piece->type == 6) // J
    {
        switch (piece->rotation)
        {
            case 0:
                map[ligne - 1][colonne - 1] = 7;
                map[ligne][colonne - 1] = 7;
                map[ligne][colonne] = 7;
                map[ligne][colonne + 1] = 7;
                break;
            case 1:
                map[ligne - 1][colonne + 1] = 7;
                map[ligne - 1][colonne] = 7;
                map[ligne][colonne] = 7;
                map[ligne + 1][colonne] = 7;
                break;
            case 2:
                map[ligne][colonne - 1] = 7;
                map[ligne][colonne] = 7;
                map[ligne][colonne + 1] = 7;
                map[ligne + 1][colonne + 1] = 7;
                break;
            case 3:
                map[ligne - 1][colonne] = 7;
                map[ligne][colonne] = 7;
                map[ligne + 1][colonne] = 7;
                map[ligne + 1][colonne - 1] = 7;
                break;
        }
        return 0;
    }
    return 0;
}