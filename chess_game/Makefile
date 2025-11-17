# --- CONFIGURATION ----------------------------------------------------

CC       = gcc
CFLAGS   = -Wall -Wextra -std=c99 -Iraylib/src
LDFLAGS  = -Lraylib/src -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

SRC_DIR  = .
OBJ_DIR  = obj
RAYLIB_DIR = raylib

SRC      = chess_game.c set_board.c show_moves.c movement.c init_game.c
OBJ      = $(SRC:%.c=$(OBJ_DIR)/%.o)

TARGET   = chess

# --- RULES ------------------------------------------------------------

all: raylib $(TARGET)

# Compilation des .o dans le dossier obj/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

# --- RAYLIB BUILD -----------------------------------------------------

raylib:
	@if [ ! -d "$(RAYLIB_DIR)" ]; then \
		echo "===> Clonage de Raylib..."; \
		git clone https://github.com/raysan5/raylib.git $(RAYLIB_DIR); \
	fi
	@if [ ! -f "$(RAYLIB_DIR)/src/libraylib.a" ]; then \
		echo "===> Compilation de Raylib..."; \
		$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP; \
	fi

# --- CLEAN ------------------------------------------------------------

clean:
	@echo "===> Nettoyage complet (obj/, binaire, raylib)..."
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
	rm -rf $(RAYLIB_DIR)

fclean: clean

re: fclean all

.PHONY: all clean fclean re raylib