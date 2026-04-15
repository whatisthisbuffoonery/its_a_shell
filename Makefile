CC := clang
CC_FLAGS := -Wall -Wextra -Werror -g
LIBFT_DIR := ../temp/uwu/libft
NAME := minishell

SRC := main.c env.c syntax.c handler.c err.c
OBJ := $(SRC:.c=.o)

all: $(OBJ)
	$(CC) $(CC_FLAGS) $(OBJ) -I$(LIBFT_DIR)/includes -L$(LIBFT_DIR) -lft -o $(NAME)

$(OBJ):%.o: %.c
	$(CC) $(CC_FLAGS) $< -I$(LIBFT_DIR)/includes -c -o $@
