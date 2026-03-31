
#include "sudoku.h"

int find_empty_cell(char **board, int *x, int *y)
{
    *x = 0;
    *y = 0;

    while (*x < 9)
    {
        while (*y < 9)
        {
            if (board[*x][*y] == '0')
            {
                return 1;
            }
            (*y)++;
        }
        (*x)++;
        *y = 0;
    }

    return 0;
}

int solve(char **board)
{
    int x, y;
    int num = 1;
    if (!find_empty_cell(board, &x, &y))
        return 1;
    while(num <= 9)
    {
        board[x][y] = num + '0';
        if (are_9_digits(board) && are_row_col_valid(board))
        {
            if (solve(board))
                return 1;
        }
        board[x][y] = '0';
        num++;
    }
    return 0;
}
void print_board(char **board)
{
    for (int i = 0; i < LINES; i++)
    {
        for (int j = 0; j < LINES; j++)
        {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int ac, char **av)
{
    char **board;
    srand(time(NULL));
    board = sudoku_maker(ac >= 2 ? av[1] : NULL);
    print_board(board);
    if (solve(board))
        printf("Le Sudoku est résolu !\n");
    else
        printf("Impossible de résoudre le Sudoku.\n");
    print_board(board);
    for (int i = 0; i < 9; i++)
    {
        free(board[i]);
    }
    free(board);
    return 0;
}
