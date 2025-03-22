#include "signals.h"

/**
 * main - Prints a description of a given signal.
 * @argc: The number of command-line arguments.
 * @argv: An array of command-line argument strings.
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int main(int argc, char *argv[])
{
	int signum;
	const char *description;

	if (argc != 2)
	{
		printf("Usage: %s <signum>\n", argv[0]);
		return (EXIT_FAILURE);
	}

	signum = atoi(argv[1]);
	description = strsignal(signum);

	if (description == NULL)
		printf("%d: Unknown signal %d\n", signum, signum);
	else
		printf("%d: %s\n", signum, description);

	return (EXIT_SUCCESS);
}
