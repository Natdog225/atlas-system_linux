CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic
TARGET = 0-hreadelf
SRCS = 0-hreadelf.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean