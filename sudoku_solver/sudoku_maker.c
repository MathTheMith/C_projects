#include "sudoku.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

char **grid_maker(void)
{
    int i = 0, j = 0;
    char **board = malloc(9 * sizeof(char *));
    if (!board)
        return NULL;

    while (i < 9)
    {
        board[i] = malloc(9 * sizeof(char));
        if (!board[i])
        {
            while (i > 0)
            {
                i--;
                free(board[i]);
            }
            free(board);
            return NULL;
        }
        j = 0;
        while (j < 9)
        {
            board[i][j] = '0';
            j++;
        }
        i++;
    }
    return board;
}

int is_valid_number(const char *str)
{
    if (!str || *str == '\0')
        return 0;
    
    while (*str)
    {
        if (!isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

void free_board(char **board)
{
    int i = 0;
    if (!board)
        return;
    
    while (i < 9)
    {
        free(board[i]);
        i++;
    }
    free(board);
}

void print_progression(int num_cells, int i)
{
    char bar[82];   
    int filled = (num_cells > 0) ? (i * 40) / num_cells : 0;                  
    for (int p = 0; p < 40; p++)                                              
        bar[p] = (p < filled) ? '#' : ' ';                                    
    bar[40] = '\0';                                                           
    printf("\r[%s] %d/%d", bar, i, num_cells);                                
}

char **sudoku_maker(char *arg)
{
    char how_many_numbs[4];
    int i = 0;
    int num_cells;
    char **board;

    if (arg && is_valid_number(arg))
    {
        num_cells = atoi(arg);
        if (num_cells >= 0 && num_cells <= 81)
        {
            printf("Nombre de chiffres : %d\n", num_cells);
            goto skip_prompt;
        }
        printf("Argument invalide (doit être entre 0 et 81).\n");
    }
    printf("Combien de chiffres voulez-vous ?\n");
    while (1)
    {
        printf("Veuillez entrer un chiffre entre 0 et 81 : ");
        if (scanf("%3s", how_many_numbs) != 1)
        {
            printf("Erreur de lecture.\n");
            while (getchar() != '\n');
            continue;
        }
        if (is_valid_number(how_many_numbs))
        {
            num_cells = atoi(how_many_numbs);
            if (num_cells >= 0 && num_cells <= 81)
                break;
        }
        printf("Entrée invalide. Essayez encore.\n");
    }
    skip_prompt:;
    board = grid_maker();
    if (!board)
    {
        printf("Erreur d'allocation mémoire.\n");
        return NULL;
    }

    if (num_cells >= 60)
    {
        printf("\nIt can take some time...\n");
        fflush(stdout);
    }
    srand(time(NULL));
    while (i < num_cells)
    {
        int random_numb = rand() % 9 + 1;
        int random_x = rand() % 9;
        int random_y = rand() % 9;

        if (board[random_x][random_y] == '0')
        {
            board[random_x][random_y] = random_numb + '0';
            if (are_row_col_valid(board) && are_9_digits(board))
            {
                char *tmp_rows[9];
                char tmp_data[9][9];
                for (int r = 0; r < 9; r++) {
                    tmp_rows[r] = tmp_data[r];
                    for (int c = 0; c < 9; c++)
                        tmp_data[r][c] = board[r][c];
                }
                if (solve(tmp_rows))
                    i++;
                else
                    board[random_x][random_y] = '0';
            }
            else
            {
                board[random_x][random_y] = '0';
            }
        }
        print_progression(num_cells, i);
    }
    printf("\n\n");
    return board;
}