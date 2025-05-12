CC = gcc
CFLAGS = -Wall -Werror -Wextra -pedantic -std=gnu99 -g # Added -g for debugging

# Betty check variables (adjust paths if necessary)
BETTY_STYLE = betty-style.pl
BETTY_DOC = betty-doc.pl

# Default target
all: strace_0 strace_1 strace_2 strace_3

strace_0: strace_0.c
	$(CC) $(CFLAGS) strace_0.c -o strace_0

strace_1: strace_1.c syscalls_64.h
	$(CC) $(CFLAGS) strace_1.c -o strace_1

strace_2: strace_2.c syscalls_64.h
	$(CC) $(CFLAGS) strace_2.c -o strace_2

strace_3: strace_3.c syscalls_64.h
	$(CC) $(CFLAGS) strace_3.c -o strace_3

clean:
	rm -f strace_0 strace_1 strace_2 strace_3 *.o

betty:
	$(BETTY_STYLE) *.c *.h
	$(BETTY_DOC) *.c *.h

.PHONY: all clean betty