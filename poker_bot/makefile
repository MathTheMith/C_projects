# Définir les variables
CC = g++
CFLAGS = -Wall -Wextra -std=c++11
INCLUDES = -Iincludes
SRCS = main.cpp \
       HandEvaluator.cpp \
       GameLogic.cpp \
       preflop.cpp \
       resultCard.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = poker_bot

# Cible par défaut (compilation du programme principal)
all: $(TARGET)

# Générer l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compilation des fichiers .cpp en .o
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Cible pour nettoyer les fichiers objets et l'exécutable
fclean:
	rm -f $(OBJS) $(TARGET)

# Cible pour recompiler tout
re: fclean all
