#include "signals.h"

/**
 * handle_sigint - Signal handler for SIGINT.
 * @signum: The signal number
 *
 * Description: Prints messages and resets the signal handler to default
 *
 */
void handle_sigint(int signum)
{
	printf("Caught %d\n", signum);
	printf("Signal received\n");
	fflush(stdout);

	signal(SIGINT, SIG_DFL);
}

/**
 * main - Sets up a SIGINT handler and suspends.
 *
 * Return: Eh?
 */
int main(void)
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return (EXIT_FAILURE);
	}

	pause(); /* Suspend indefinitely until a signal is received */
}
