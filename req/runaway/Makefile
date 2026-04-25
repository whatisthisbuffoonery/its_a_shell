CC := clang
CC_FLAGS := -Wall -Wextra -Werror -g
LIBFT_DIR := libft
SRC_DIR := ..
NAME := minishell

LIBS := -L$(LIBFT_DIR) -lreadline -lhistory -lft
INCLUDES := -I$(LIBFT_DIR)
SRC_FILE := merge_sort.c main.c err.c cleanup.c signal.c \
			env_init.c env_utils.c shnode_utils.c \
			cmd_init.c cmd_utils.c checker.c \
			muh_print.c \
			ast_check.c \
			word.c parse.c \

OBJ := $(SRC_FILE:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(MAKE) -C $(LIBFT_DIR)
	$(CC) $(CC_FLAGS) $(OBJ) $(LIBS) $(INCLUDES) -o $(NAME)

$(OBJ): %.o: $(SRC_DIR)/%.c
	$(CC) $(CC_FLAGS) -c $< $(INCLUDES) -o $@

#exclude libs here

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
