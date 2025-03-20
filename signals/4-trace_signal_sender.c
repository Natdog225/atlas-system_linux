#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handle_sigquit(int signum, siginfo_t *info, void *ucontext)
{
	(void)ucontext; // Suppress unused para warning
	printf("SIGQUIT sent by %d\n", info->si_pid);
}

int trace_signal_sender(void)
{
	struct sigaction sa;

	sa.sa_sigaction = handle_sigquit; // for extra info
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;

	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return -1;
	}
	return 0;
}