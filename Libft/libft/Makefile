# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/28 21:29:55 by achew             #+#    #+#              #
#    Updated: 2026/06/18 22:55:09 by achew            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = cc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I .

SRC = absf.c \
      ft_abs.c \
      ft_atohu_ind.c \
      ft_atoi.c \
      ft_atoi_ind.c \
      ft_bzero.c \
      ft_calloc.c \
      ft_ftoa.c \
      ft_intset.c \
      ft_isalnum.c \
      ft_isalpha.c \
      ft_isascii.c \
      ft_isdigit.c \
      ft_ishex.c \
      ft_isprint.c \
      ft_isquote.c \
      ft_isspace.c \
      ft_itoa.c \
      ft_memchr.c \
      ft_memcmp.c \
      ft_memcpy.c \
      ft_memmove.c \
      ft_memset.c \
      ft_ptrlen.c \
      ft_putchar.c \
      ft_putchar_fd.c \
      ft_putendl_fd.c \
      ft_putnbr.c \
      ft_putnbr_fd.c \
      ft_putstr.c \
      ft_putstr_fd.c \
      ft_split.c \
      ft_strchr.c \
      ft_strcmp.c \
      ft_strdup.c \
      ft_striteri.c \
      ft_strjoin.c \
      ft_strlcat.c \
      ft_strlcpy.c \
      ft_strlen.c \
      ft_strmapi.c \
      ft_strncmp.c \
      ft_strndup.c \
      ft_strnstr.c \
      ft_strpart.c \
      ft_strrchr.c \
      ft_strtrim.c \
      ft_substr.c \
      ft_swap.c \
      ft_swapf.c \
      ft_tolower.c \
      ft_toupper.c \
      ht_init.c \
      malloc_cond.c \
      max2.c \
      min.c \
      minf.c \
      parsed_argsplit.c \
      strlen_null.c \

HDR = libft.h

SRC_BONUS = ft_lstadd_back.c \
            ft_lstadd_front.c \
            ft_lstclear.c \
            ft_lstdelone.c \
            ft_lstiter.c \
            ft_lstiter_chk.c \
            ft_lstlast.c \
            ft_lstmap.c \
            ft_lstnew.c \
            ft_lstnew_chk.c \
            ft_lstsize.c \

OBJ = $(SRC:.c=.o)

OBJ_BONUS = $(SRC_BONUS:.c=.o)

NAME = libft.a

all: $(NAME)

bonus: $(NAME)

ifneq ($(filter bonus re_bonus,$(MAKECMDGOALS)),)
EXTRA_OBJ = $(OBJ_BONUS)
else
EXTRA_OBJ =
endif

$(NAME): $(OBJ) $(EXTRA_OBJ)
	ar rcs $@ $?

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) $(OBJ_BONUS)

fclean: clean
	rm -f $(NAME)

re: fclean all

re_bonus: fclean bonus

.PHONY: clean fclean re bonus re_bonus
