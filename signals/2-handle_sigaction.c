#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int signum)
{
	printf("Gotcha! [%d]\n", signum);
}

int handle_sigaction(void)
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask); // Block no other signals during handling
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return -1;
	}
	return 0;
}