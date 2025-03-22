#include "signals.h"

/**
 * main - Sends the SIGINT signal to a process.
 * @argc: The number of command-line args
 * @argv: An array of command-line arg strings.
 *
 * Return: EXIT_SUCCESS when it succeeds failure when it doesn't.
 */
int main(int argc, char *argv[])
{
	pid_t pid;

	if (argc != 2)
	{
		printf("Usage: %s <pid>\n", argv[0]);
		return (EXIT_FAILURE);
	}

	pid = atoi(argv[1]); /* Convert PID string to integer */

	if (kill(pid, SIGINT) == -1)
	{
		perror("kill");
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
