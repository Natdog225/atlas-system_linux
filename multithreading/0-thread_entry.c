#include <stdio.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * thread_entry - Prints a string and terminates the calling thread.
 * @arg: A void pointer to a null-terminated string.
 *
 * Description: This function casts `arg` to a `char *` and prints the
 * string it points to, followed by a newline. It then
 * calls `pthread_exit(NULL)` to terminate the calling thread.
 * This means if called by `main` directly, `main` will exit.
 *
 * Return: Does not return to the caller as `pthread_exit()` is called.
 * The thread's exit status is `NULL`.
 */
void *thread_entry(void *arg)
{
	char *str_arg = (char *)arg;

	printf("%s\n", str_arg);

	pthread_exit(NULL);

	/* This line is unreachable due to pthread_exit() */
	return (NULL);
}
