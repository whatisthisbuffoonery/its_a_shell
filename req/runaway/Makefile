CC := clang
CC_FLAGS := -Wall -Wextra -Werror -g
LIBFT_DIR := libft
SRC_DIR := ..
NAME := minishell

LIBS := $(LIBFT_DIR) -lreadline -lhistory -lft
INCLUDES := $(LIBFT_DIR)/includes
# SRC_FILE := main.c err.c cleanup.c signal.c arg_clean.c \
# 			env_init.c shnode_utils.c \
# 			tok_init.c tok_utils.c checker.c \
# 			muh_print.c \
# 			ast_check.c ast_init.c ast_logic.c ast_redir.c ast_print.c \
# 			word.c \
# 			env_utils.c exec.c exec_cleanup.c pipe_utils.c argv.c envp.c expand.c redir.c child.c \
# 			echo.c exit.c env.c unset.c export.c cd.c other_cd.c pwd.c

SRC_FILE := main.c err.c signal.c word.c update.c cleanup.c checker.c \
			ast_check.c ast_init.c ast_logic.c ast_redir.c \
			expand.c expand_utils.c expand_handlers.c field.c glob_utils.c \
			echo.c exit.c pwd.c export.c export_helpers.c unset.c env.c cd.c cd_utils.c \
			argv.c redir.c envp.c arg_cleanup.c \
			child.c pipe_utils.c exec.c exec_cleanup.c \
			env_init.c shnode_cleanup.c shnode_utils.c find.c \
			tok_init.c tok_utils.c 

OBJ := $(SRC_FILE:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(MAKE) -C $(LIBFT_DIR)
	$(CC) $(CC_FLAGS) $(OBJ) -L$(LIBS) -I$(INCLUDES) -o $(NAME)

$(OBJ): %.o: $(SRC_DIR)/%.c $(INCLUDES)/libft.h
	$(CC) $(CC_FLAGS) -c $< -I$(INCLUDES) -o $@

#exclude libs here

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
