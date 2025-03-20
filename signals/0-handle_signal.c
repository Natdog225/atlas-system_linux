#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int signum)
{
	printf("Gotcha! [%d]\n", signum);
}

int handle_signal(void)
{
	if (signal(SIGINT, handle_sigint) == SIG_ERR)
	{
		perror("signal");
		return -1;
	}
	return 0;
}