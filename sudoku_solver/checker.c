
#include "sudoku.h"

bool are_9_digits(char **board)
{
    int x, y;
    int seen[9];
    int start_x, start_y, i;
    int num = 0;

    start_x = 0;
    while (start_x < 9)
    {
        start_y = 0;
        while (start_y < 9)
        {
            i = 0;
            while (i < 9)
            {
                seen[i] = 0;
                i++;
            }
            x = start_x;
            while (x < start_x + 3) {
                y = start_y;
                while (y < start_y + 3)
                {
                    if (board[x][y] != '0')
                    {
                        num = board[x][y] - '1';

                        if (seen[num] == 1)
                            return false;
                        seen[num] = 1;
                    }
                    y++;
                }
                x++;
            }
            start_y += 3;
        }
        start_x += 3;
    }
    return true;
}

bool are_row_col_valid(char **board)
{
    int x, y, num;
    int seen[9];

    x = 0;
    while (x < 9)
    {
        y = 0;
        while (y < 9)
        {
            seen[y] = 0;
            y++;
        }
        y = 0;
        while (y < 9)
        {
            if (board[x][y] != '0')
            {
                num = board[x][y] - '1';
                if (seen[num] == 1)
                    return false;
                seen[num] = 1;
            }
            y++;
        }
        x++;
    }

    y = 0;
    while (y < 9)
    {
        x = 0;
        while (x < 9)
        {
            seen[x] = 0;
            x++;
        }
        x = 0;
        while (x < 9)
        {
            if (board[x][y] != '0')
            {
                num = board[x][y] - '1';
                if (seen[num] == 1)
                    return false;
                seen[num] = 1;
            }
            x++;
        }
        y++;
    }

    return true;
}
