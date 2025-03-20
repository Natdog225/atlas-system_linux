#include <signal.h>
#include <stdio.h>

void (*current_handler_sigaction(void))(int)
{
	struct sigaction sa;

	// Use sigaction with a NULL new action to retrieve current actiion
	if (sigaction(SIGINT, NULL, &sa) == -1)
	{
		perror("sigaction");
		return NULL;
	}
	return sa.sa_handler;
}