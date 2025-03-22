#include <signal.h>
/**
 * current_handler_signal - retrieves the current handler of the signal SIGINT
 *
 * Return: void pointer of __sighandler_t
 */
void (*current_handler_signal(void))(int)
{
	void (*handler)(int);

	handler = signal(SIGINT, SIG_DFL);
	signal(SIGINT, handler);

	return (handler);
}
