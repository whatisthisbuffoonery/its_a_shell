# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/28 21:29:55 by achew             #+#    #+#              #
#    Updated: 2026/04/17 13:57:58 by achew            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #
# **************************************************************************** #
#                                   CONFIG                                     #
# **************************************************************************** #

NAME        = minishell 

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -g -O0
INCLUDES    = -I./Libft

LIBFT_DIR   := Libft

LIBFT_LIB   := ./Libft/libft/libft.a
GNL_LIB     := ./Libft/gnl/libftgnl.a
PRINTF_LIB  := ./Libft/printf/libftprintf.a

LIBS        := $(LIBFT_LIB) $(GNL_LIB) $(PRINTF_LIB) -lreadline -lhistory

HDR 		:= h_minishell.h
HDR_BONUS	:= $(HDR)

# **************************************************************************** #
#                                   SOURCES                                    #
# **************************************************************************** #

SRC         = merge_sort.c \
			  main.c \
			  checker.c \
			  err.c \
			  cleanup.c \
			  env_init.c \
			  env_utils.c \
			  cmd_init.c \
			  print.c \
			  shnode_utils.c \
			  signal.c \
			  ft_strcmp.c \
			  ft_isquote.c \
			  ft_strndup.c \
			  malloc_cond.c \
			  ft_isspace.c \
			  ft_putstr.c \
			  ft_putchar.c \

SRC_BONUS   = $(SRC)

OBJ         := $(SRC:.c=.o)
OBJ_BONUS   := $(SRC_BONUS:.c=.o)

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
EXTRA_OBJ = $(OBJ_BONUS)
EXTRA_HDR = $(HDR_BONUS)
OBJ =
HDR =
else
EXTRA_OBJ =
EXTRA_HDR =
endif

$(NAME): $(OBJ) $(EXTRA_OBJ) $(HDR)
	$(CC) $(CFLAGS) $(INCLUDES) \
		$(OBJ) $(EXTRA_OBJ) \
		$(LIBS) \
		-o $(NAME)

# -------------------- Compilation --------------------

%.o: %.c $(LIBS) $(HDR) $(EXTRA_HDR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# **************************************************************************** #
#                                   CLEAN                                      #
# **************************************************************************** #

clean:
	rm -f $(OBJ) $(OBJ_BONUS)
	$(MAKE) -C $(LIBFT_DIR) clean

fclean:
	rm -f $(OBJ) $(OBJ_BONUS)
	rm -f $(NAME)
	$(MAKE) -C $(LIBFT_DIR) fclean

re: fclean all
re_bonus: fclean bonus

.PHONY: all clean fclean re bonus re_bonus
