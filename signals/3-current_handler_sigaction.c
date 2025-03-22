#include <signal.h>
#include <stdio.h>
/**
 * current_handler_sigaction - handler for STUFF
 *
 *
 * Return: sa.sa_handler
 */
void (*current_handler_sigaction(void))(int)
{
	struct sigaction sa;

	if (sigaction(SIGINT, NULL, &sa) == -1)
	{
		perror("sigaction");
		return (NULL);
	}
	return (sa.sa_handler);
}
