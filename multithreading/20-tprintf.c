#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include "multithreading.h"

/* Global mutex for tprintf, initialized by constructor */
static pthread_mutex_t tprintf_mutex;

/**
 * init_tprintf_mutex - GCC constructor to initialize the tprintf mutex.
 *
 * Description: Initializes `tprintf_mutex` before main() is called.
 */
void __attribute__((constructor)) init_tprintf_mutex(void)
{
	pthread_mutex_init(&tprintf_mutex, NULL);
}

/**
 * destroy_tprintf_mutex - GCC destructor to destroy the tprintf mutex.
 *
 * Description: Destroys `tprintf_mutex` after main() completes or program exits.
 */
void __attribute__((destructor)) destroy_tprintf_mutex(void)
{
	pthread_mutex_destroy(&tprintf_mutex);
}

/**
 * tprintf - Thread-safe printf; prefixes output with calling thread ID.
 * @format: Printf-style format string.
 * @...:    Arguments for format string.
 *
 * Description: Prepends thread ID (e.g., "[<tid>] ") to formatted output.
 * Uses a mutex to ensure thread-safe printing to stdout.
 *
 * Return: Total number of characters printed, or a negative value on error.
 */
int tprintf(char const *format, ...)
{
	pthread_t tid;
	int chars_printed_total = 0;
	int ret_id = 0;
	int ret_msg = 0;
	va_list args;

	if (pthread_mutex_lock(&tprintf_mutex) != 0)
		return (-1); /* Failed to lock mutex */

	tid = pthread_self();
	ret_id = printf("[%lu] ", (unsigned long)tid);

	if (ret_id < 0)
	{
		pthread_mutex_unlock(&tprintf_mutex);
		return (ret_id); /* Error printing thread ID */
	}

	va_start(args, format);
	ret_msg = vprintf(format, args);
	va_end(args);

	if (ret_msg < 0)
	{
		pthread_mutex_unlock(&tprintf_mutex);
		return (ret_msg); /* Error printing formatted message */
	}

	fflush(stdout);

	pthread_mutex_unlock(&tprintf_mutex);

	chars_printed_total = ret_id + ret_msg;
	return (chars_printed_total);
}
