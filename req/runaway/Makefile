CC := clang
CC_FLAGS := -Wall -Wextra -Werror -g
LIBFT_DIR := libft
SRC_DIR := ..
NAME := minishell

LIBS := -L$(LIBFT_DIR) -lreadline -lhistory -lft
INCLUDES := -I$(LIBFT_DIR)/includes -I$(SRC_DIR)
HEADERS := $(LIBFT_DIR)/includes/libft.h $(SRC_DIR)/h_minishell.h

SRC_FILE := main.c loop.c buf_check.c err.c signal.c word.c update.c cleanup.c checker.c \
			ast_check.c ast_init.c ast_logic.c ast_redir.c \
			expand.c expand_utils.c expand_handlers.c field.c glob_utils.c \
			echo.c exit.c pwd.c export.c export_helpers.c unset.c env.c cd.c cd_utils.c \
			argv.c path.c redir.c heredoc.c envp.c exec.c arg_cleanup.c \
			child.c pipe_utils.c do_list.c exec_cleanup.c \
			env_init.c shnode_cleanup.c shnode_utils.c find.c \
			tok_init.c tok_utils.c tok_checker.c

OBJ := $(SRC_FILE:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(MAKE) -C $(LIBFT_DIR)
	$(CC) $(CC_FLAGS) $(OBJ) $(LIBS) $(INCLUDES) -o $(NAME)

$(OBJ): %.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CC_FLAGS) -c $< $(INCLUDES) -o $@

#exclude libs here

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
