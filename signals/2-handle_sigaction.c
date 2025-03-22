#include <stdio.h>
#include <signal.h>
#include <unistd.h>
/**
 * handle_sigint - handler for SIGINTs
 * @signum: signal number
 *
 * Return: void
 */
void handle_sigint(int signum)
{
	printf("Gotcha! [%d]\n", signum);
	fflush(stdout);
}
/**
 * handle_sigaction - handler for SIGINTs
 * @sig: signal number
 *
 * Return: void
 */
int handle_sigaction(void)
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return (-1);
	}
	return (0);
}
