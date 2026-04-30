# --- CONFIGURATION ----------------------------------------------------

CC       = gcc
CFLAGS   = -Wall -Wextra -std=c99 -Iraylib/src -g -fsanitize=address -MMD -MP
LDFLAGS_SAN = -fsanitize=address
LDFLAGS  = -Lraylib/src -lraylib -lm -ldl -lpthread -lGL -lrt -lX11

SRC_DIR  = .
OBJ_DIR  = obj
RAYLIB_DIR = raylib

SRC      = chess_game.c \
			 set_board.c \
			 show_moves.c \
			 movement.c \
			 init_game.c \
			 engine/init_engine.c \
			 engine/check_moves.c \

OBJ      = $(SRC:%.c=$(OBJ_DIR)/%.o)
DEP      = $(OBJ:%.o=%.d)

TARGET   = chess

# --- COLORS -----------------------------------------------------------

RESET   = \033[0m
BOLD    = \033[1m
GREEN   = \033[32m
CYAN    = \033[36m
YELLOW  = \033[33m
RED     = \033[31m

# --- RULES ------------------------------------------------------------

all: raylib $(TARGET)
	@echo "$(BOLD)$(GREEN)All compiled$(RESET)"

# Compilation des .o dans le dossier obj/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "$(CYAN)CC$(RESET)  $<"
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ)
	@echo "$(YELLOW)LD$(RESET)  $(TARGET)"
	@$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS) $(LDFLAGS_SAN)

# --- RAYLIB BUILD -----------------------------------------------------

raylib:
	@if [ ! -d "$(RAYLIB_DIR)" ]; then \
		echo "$(YELLOW)===> Clonage de Raylib...$(RESET)"; \
		git clone https://github.com/raysan5/raylib.git $(RAYLIB_DIR); \
	fi
	@if [ ! -f "$(RAYLIB_DIR)/src/libraylib.a" ]; then \
		echo "$(YELLOW)===> Compilation de Raylib...$(RESET)"; \
		$(MAKE) -C $(RAYLIB_DIR)/src PLATFORM=PLATFORM_DESKTOP; \
	fi

# --- CLEAN ------------------------------------------------------------

clean:
	@echo "$(RED)RM$(RESET)  $(OBJ_DIR)/ $(TARGET)"
	@rm -rf $(OBJ_DIR)
	@rm -f $(TARGET)

fclean: clean
	@echo "$(RED)RM$(RESET)  $(RAYLIB_DIR)/"
	@rm -rf $(RAYLIB_DIR)

re: fclean all

-include $(DEP)

.PHONY: all clean fclean re raylib
