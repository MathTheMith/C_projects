#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 22735
#define BUFFER_SIZE 30
#define MAX_TRIES 6
#define MAX_WORD_LENGTH 30
#define COLOR_LENGTH 20

typedef struct {
    char colored_word[MAX_WORD_LENGTH][COLOR_LENGTH];
    int word_length;
} Try;

char *get_random_line(const char *filename, int target_line)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Erreur ouverture fichier");
        return NULL;
    }

    char buffer[MAX_WORD_LENGTH];
    int current_line = 1;

    while (fgets(buffer, sizeof(buffer), file))
    {
        if (current_line == target_line)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            char *line = malloc(strlen(buffer) + 1);
            strcpy(line, buffer);
            fclose(file);
            return line;
        }
        current_line++;
    }

    fclose(file);
    return NULL;
}
char *print_answer(char *word_to_find, char *guess, Try history[], int current_try)
{
    int len = strlen(word_to_find);
    int used_target[MAX_WORD_LENGTH] = {0};
    int used_guess[MAX_WORD_LENGTH] = {0};
    char temp[COLOR_LENGTH];

    history[current_try].word_length = len;

    for (int i = 0; i < len; i++)
    {
        if (guess[i] == word_to_find[i])
        {
            sprintf(temp, "\033[32m%c\033[0m", guess[i]);
            strcpy(history[current_try].colored_word[i], temp);
            used_target[i] = 1;
            used_guess[i] = 1;
        }
    }

    for (int i = 0; i < len; i++)
    {
        if (used_guess[i])
            continue;

        int found = 0;

        for (int j = 0; j < len; j++)
        {
            if (!used_target[j] && guess[i] == word_to_find[j])
            {
                sprintf(temp, "\033[33m%c\033[0m", guess[i]);
                strcpy(history[current_try].colored_word[i], temp);
                used_target[j] = 1;
                found = 1;
                break;
            }
        }

        if (!found)
        {
            sprintf(temp, "\033[31m%c\033[0m", guess[i]);
            strcpy(history[current_try].colored_word[i], temp);
        }
    }

    for (int k = 0; k <= current_try; k++)
    {
        for (int i = 0; i < history[k].word_length; i++)
        {
            printf("%s", history[k].colored_word[i]);
        }
        printf("\n");
    }

    return NULL;
}

void wordle(char *word_to_find)
{
    char guess[MAX_WORD_LENGTH];
    int tries = MAX_TRIES;
    int current_try = 0;
    Try history[MAX_TRIES] = {0};;

    printf("BIENVENUE AU WORDLE \nQuel est votre premier mot ? \nLe mot est de %ld caractères et vous avez %d essais.\n", 
           strlen(word_to_find), tries);

    while (tries > 0)
    {
        printf("Entrez un mot : ");
        scanf("%29s", guess);

        if (strlen(guess) != strlen(word_to_find))
        {
            printf("Il n'y a pas %ld caractères\n", strlen(word_to_find));
            continue;
        }
        print_answer(word_to_find, guess, history, current_try);
        if (strcmp(guess, word_to_find) == 0)
        {
            printf("Félicitations! Vous avez trouvé le mot : %s\n", word_to_find);
            break;
        }
        tries--;
        current_try++;
        
        if (tries > 0)
            printf("Ce n'est pas le bon mot. Il vous reste %d essais\n", tries);
    }
    if (tries == 0)
    {
        printf("Vous avez perdu, le mot était %s\n", word_to_find);
    }
}
int main()
{
    char *word_to_find;
    const char *filename = "liste_mots.txt";
    srand(time(NULL));
    int random_line = rand() % MAX_LINE_LENGTH + 1;

    word_to_find = get_random_line(filename, random_line);
    if (word_to_find == NULL) {
        printf("Erreur : ligne non trouvée.\n");
        return EXIT_FAILURE;
    }
    printf("%s\n", word_to_find);
    wordle(word_to_find);
    free(word_to_find);

    return EXIT_SUCCESS;
}
