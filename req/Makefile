CC := clang
CC_FLAGS := -Wall -Wextra -Werror -g
LIBFT_DIR := ../../temp/uwu/libft
NAME := minishell

LIBS := -L$(LIBFT_DIR) -lreadline -lhistory -lft
INCLUDES := -I$(LIBFT_DIR)
SRC := merge_sort.c main.c checker.c err.c cleanup.c \
	   env_init.c env_utils.c cmd_init.c \
	   print.c shnode_utils.c signal.c

OBJ := $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(MAKE) -C $(LIBFT_DIR)
	$(CC) $(CC_FLAGS) $(OBJ) $(LIBS) $(INCLUDES) -o $(NAME)

$(OBJ): %.o: %.c
	$(CC) $(CC_FLAGS) -c $< $(INCLUDES) -o $@

#exclude libs here

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
