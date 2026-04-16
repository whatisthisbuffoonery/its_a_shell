CC := clang
CC_FLAGS := -Wall -Wextra -Werror
LIBFT_DIR := ../../temp/uwu/libft

LIBS := -L$(LIBFT_DIR) -lreadline -lhistory -lft
INCLUDES := -I$(LIBFT_DIR)
SRC := merge_sort.c main.c

all:
	$(CC) $(CC_FLAGS) $(SRC) $(LIBS) $(INCLUDES)
