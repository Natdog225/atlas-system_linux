CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=gnu89
# CFLAGS += -g # Uncomment for debugging symbols

# Executable Name
NAME = hnm

# Source Files
SRCS = main.c \
       endian_swap_64.c \
       endian_swap_32.c \
       symbol_type.c \
       process_elf64.c \
       process_elf32.c

# Object Files
OBJS = $(SRCS:.c=.o)

# Header File (dependency tracking)
HDR = hnm.h

# Default rule: Build the executable
all: $(NAME)

# Rule to link object files into the executable
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# Rule to compile .c files into .o files
%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to remove object files
clean:
	rm -f $(OBJS)

# Rule to remove object files and the executable
fclean: clean
	rm -f $(NAME)

# Rule to recompile everything
re: fclean all

# Declare non-file targets
.PHONY: all clean fclean re
