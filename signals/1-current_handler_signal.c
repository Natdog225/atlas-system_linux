#include <signal.h>

void (*current_handler_signal(void))(int)
{
	void (*handler)(int);

	// signal(SIGINT, SIG_DFL) gets the current handler then temp sets to defualt
	handler = signal(SIGINT, SIG_DFL);
	signal(SIGINT, handler);

	return handler;
}