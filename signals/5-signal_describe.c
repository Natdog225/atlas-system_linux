// 5-signal_describe.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <signum>\n", argv[0]);
		return EXIT_FAILURE;
	}
	int signum = atoi(argv[1]);
	const char *description = strsignal(signum);
	if (description == NULL)
		printf("%d: Unknown signal %d\n", signum, signum);
	else
		printf("%d: %s\n", signum, description);
	return EXIT_SUCCESS;
}