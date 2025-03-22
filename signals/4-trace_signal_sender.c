#include <signals.h>

/**
 * handle_sigquit - Signal handler for SIGQUIT.
 * @signum: The signal number (should be SIGQUIT).
 * @info: Pointer to a siginfo_t structure with signal information.
 * @ucontext: Pointer to a ucontext_t structure (unused).
 *
 * Description: Prints the PID of the process that sent the SIGQUIT signal.
 */
void handle_sigquit(int signum, siginfo_t *info, void *ucontext)
{
	(void)ucontext; /* Suppress unused parameter warning for ucontext */

	if (signum == SIGQUIT)
	{
		printf("SIGQUIT sent by %d\n", info->si_pid);
	}
	else
	{
		fprintf(stderr, "Unexpected signal: %d\n", signum);
	}
}

/**
 * trace_signal_sender - Sets up a signal handler for SIGQUIT.
 *
 * Return: 0 on success, -1 on error.
 */
int trace_signal_sender(void)
{
	struct sigaction sa;

	sa.sa_sigaction = handle_sigquit; /* Use sa_sigaction for extra info */
	sigemptyset(&sa.sa_mask);		  /* Block no other signals during handling */
	sa.sa_flags = SA_SIGINFO;		  /* Get extra info in the handler */

	if (sigaction(SIGQUIT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return (-1);
	}
	return (0);
}
