#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/**
 * sigint_handler - handler specifically for SIGINT
 * @signal: signal number
 */
void sigint_handler(int signal)
{
	if (signal == SIGINT)
		printf("Gotcha! [%d]\n", SIGINT);
	fflush(stdout);
}

/**
 * handle_signal - a function that sets a handler for the signal SIGINT
 *
 * Return: 0 on success, -1 on error
 */
int handle_signal(void)
{
	__sighandler_t signal_rtn;

	signal_rtn = signal(SIGINT, sigint_handler);
	if (signal_rtn == SIG_ERR)
		return (-1);
	return (0);
}