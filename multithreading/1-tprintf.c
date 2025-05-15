#include <stdio.h>
#include <pthread.h>
#include <stdarg.h>
#include "multithreading.h"

/**
 * tprintf - Prints formatted string prefixed with calling thread ID.
 * @format: Printf-style format string.
 * @...:    Arguments for format string.
 *
 * Description: Prepends current thread ID (e.g., "[<tid>] ") to the
 * formatted output. Uses vprintf for the main printing.
 * This version is not mutex-protected.
 *
 * Return: Total number of characters printed, or a negative value on error.
 */
int tprintf(char const *format, ...)
{
	pthread_t tid;
	int chars_printed = 0;
	int ret;
	va_list args;

	tid = pthread_self();

	ret = printf("[%lu] ", (unsigned long)tid);
	if (ret < 0)
		return (ret);
	chars_printed += ret;

	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args); /* Clean up va_list soon after use */

	if (ret < 0)
		return (ret); /* vprintf failed */
	chars_printed += ret;

	fflush(stdout); /* Ensure immediate output */

	return (chars_printed);
}
