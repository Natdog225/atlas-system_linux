CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
TARGET = 0-hreadelf
SRCS = 0-hreadelf.c elf_utils.c elf_utils.h

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean