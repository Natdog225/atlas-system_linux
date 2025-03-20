#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handle_sigint(int signum)
{
	printf("Caught %d\n", signum);
	printf("Signal received\n");
	fflush(stdout); // Ensure output is flushed before leaving like a proper guest
	_exit(EXIT_SUCCESS);
}

int main()
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return EXIT_FAILURE;
	}

	pause(); // Suspend indefinitely until sig

	return EXIT_SUCCESS;
}