# Makefile for the todo_api_0 project

CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic
SRC = todo_api_0.c
TARGET = todo_api_0

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Explicit rule as requested by the project
todo_api_0: $(TARGET)

clean:
	rm -f $(TARGET)