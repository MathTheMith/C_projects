# --- CONFIGURATION ----------------------------------------------------

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -Isrc/raylib/src
LDFLAGS = -Lsrc/raylib/src -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

SRC_DIR = src
OBJ_DIR = obj
RAYLIB_DIR = src/raylib

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/game/game_logic.c \
       $(SRC_DIR)/game/input_handler.c \
       $(SRC_DIR)/render/draw_game.c \
       $(SRC_DIR)/render/draw_pieces.c \
       $(SRC_DIR)/pieces/piece_collision.c \
       $(SRC_DIR)/pieces/piece_lock.c \
       $(SRC_DIR)/pieces/piece_rotation.c \
       $(SRC_DIR)/utils/bag_system.c \
       $(SRC_DIR)/utils/line_clear.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = tetris

# --- RULES ------------------------------------------------------------

all: raylib $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "===> Build complete: ./$(TARGET)"

# Règle générique pour compiler les .c en .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- RAYLIB BUILD -----------------------------------------------------

raylib:
	@if [ ! -d "$(RAYLIB_DIR)" ]; then \
		echo "===> Clonage de Raylib..."; \
		git clone https://github.com/raysan5/raylib.git $(RAYLIB_DIR); \
	fi
	@if [ ! -f "$(RAYLIB_DIR)/src/libraylib.a" ]; then \
		echo "===> Compilation de Raylib..."; \
		$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP; \
	else \
		echo "===> Raylib déjà compilé"; \
	fi

# --- CLEAN ------------------------------------------------------------

clean:
	@echo "===> Nettoyage (obj/, binaire, raylib)..."
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
	rm -rf $(RAYLIB_DIR)

fclean: clean

re: fclean all

# --- RUN --------------------------------------------------------------

run: all
	./$(TARGET)

.PHONY: all clean fclean re raylib run