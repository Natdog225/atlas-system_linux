#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s <pid>\n", argv[0]);
		return EXIT_FAILURE;
	}

	pid_t pid = atoi(argv[1]); // Convert PID string to integer

	if (kill(pid, SIGINT) == -1)
	{
		perror("kill");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}