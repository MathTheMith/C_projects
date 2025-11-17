#ifndef SUDOKU_H
# define SUDOKU_H

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1000000
# endif

# ifndef LINES
#  define LINES 9
# endif

void	read_and_store(int fd, char ***board);
bool are_9_digits(char **board);
bool are_row_col_valid(char **board);
int find_empty_cell(char **board, int *x, int *y);
int solve(char **board);
char **sudoku_maker();



#endif
