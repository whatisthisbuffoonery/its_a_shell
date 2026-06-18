# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/28 21:29:55 by achew             #+#    #+#              #
#    Updated: 2026/06/18 23:43:34 by achew            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #
# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME        := minishell 

CC          := cc
CFLAGS      := -Wall -Wextra -Werror -g -O0

LIBFT_DIR   := ./Libft

LIBFT_LIB   := $(LIBFT_DIR)/libft/libft.a
GNL_LIB     := $(LIBFT_DIR)/gnl/libftgnl.a
PRINTF_LIB  := $(LIBFT_DIR)/printf/libftprintf.a

INCLUDES    := -I$(LIBFT_DIR)/libft -Iinclude
LIBS        := $(LIBFT_LIB) $(GNL_LIB) $(PRINTF_LIB) -lreadline -lhistory

HDR 		:= include/h_minishell.h
HDR_BONUS	:= $(HDR)

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #
SRC_DIR     := src/
OBJ_DIR     := obj/

SRC         = main.c loop.c buf_check.c err.c signal.c word.c update.c cleanup.c checker.c \
			  ast_check.c ast_extras.c ast_init.c ast_logic.c ast_redir.c \
			  expand.c expand_utils.c expand_handlers.c field.c glob_utils.c \
			  echo.c exit.c pwd.c export.c export_helpers.c unset.c env.c cd.c cd_utils.c \
			  argv.c path.c argv_empty.c redir.c heredoc.c envp.c exec.c arg_cleanup.c \
			  child.c pipe_utils.c do_list.c exec_cleanup.c \
			  env_init.c shnode_cleanup.c shnode_utils.c find.c \
			  tok_init.c tok_utils.c tok_checker.c

SRC_BONUS   = $(SRC)

OBJ         := $(addprefix $(OBJ_DIR), $(SRC:.c=.o))
OBJ_BONUS   := $(addprefix $(OBJ_DIR), $(SRC_BONUS:.c=.o))

#OBJ         := $(SRC:.c=.o)
#OBJ_BONUS   := $(SRC_BONUS:.c=.o)

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: libft $(NAME)

bonus: libft $(NAME)

# -------------------- Submodules --------------------

.PHONY: libft

libft:
	$(MAKE) -C $(LIBFT_DIR) bonus

# -------------------- Main binary --------------------
ifneq ($(filter bonus re_bonus,$(MAKECMDGOALS)),)
LINK_OBJ = $(OBJ_BONUS)
LINK_HDR = $(HDR_BONUS)
else
LINK_OBJ = $(OBJ)
LINK_HDR = $(HDR)
endif

$(NAME): $(LINK_OBJ) $(LINK_HDR) $(LIBS)
	$(CC) $(CFLAGS) $(INCLUDES) $(LINK_OBJ) $(LIBS) -o $(NAME)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c $(LINK_HDR) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

#ifneq ($(filter bonus re_bonus,$(MAKECMDGOALS)),)
#EXTRA_OBJ = $(OBJ_BONUS)
#EXTRA_HDR = $(HDR_BONUS)
#OBJ =
#HDR =
#else
#EXTRA_OBJ =
#EXTRA_HDR =
#endif

#$(NAME): $(OBJ) $(EXTRA_OBJ) $(HDR) $(EXTRA_HDR) $(LIBS)
#	$(CC) $(CFLAGS) $(INCLUDES) \
#		$(OBJ) $(EXTRA_OBJ) \
#		$(LIBS) \
#		-o $(NAME)

# -------------------- Compilation --------------------

#%.o: %.c $(LIBS) $(HDR) $(EXTRA_HDR)
#	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# **************************************************************************** #
#                                   CLEAN                                      #
# **************************************************************************** #

clean:
	rm -f $(OBJ) $(OBJ_BONUS)
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean:
	rm -f $(OBJ) $(OBJ_BONUS)
	rm -rf $(OBJ_DIR)
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all
re_bonus: fclean bonus

.PHONY: all clean fclean re bonus re_bonus
