CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -framework elf
TARGET = 0-hreadelf
SRCS = 0-hreadelf.c  # Add other .c files here if you have them

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
