# --- CONFIGURATION ----------------------------------------------------

CC       = gcc
CFLAGS   = -Wall -Wextra -std=c99 -O2
LDFLAGS  = 

SRC_DIR  = .
OBJ_DIR  = obj

SRC      = checker.c sudoku_maker.c sudoku_solver.c
OBJ      = $(SRC:%.c=$(OBJ_DIR)/%.o)

TARGET   = sudoku

# --- RULES ------------------------------------------------------------

all: $(TARGET)

# Compilation des .o dans le dossier obj/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)
	@echo "===> Compilation terminée : ./$(TARGET)"

# --- CLEAN ------------------------------------------------------------

clean:
	@echo "===> Nettoyage (obj/, binaire)..."
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)

fclean: clean

re: fclean all

# --- RUN --------------------------------------------------------------

run: all
	./$(TARGET)

.PHONY: all clean fclean re run
