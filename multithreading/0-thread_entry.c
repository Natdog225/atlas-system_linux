#include <stdio.h>
#include "multithreading.h"
#include <pthread.h> // For pthread_exit

/**
 * thread_entry - Serves as the entry point to a new thread.
 * @arg: A pointer to a string that must be printed, followed by a new line.
 *
 * Description: This function takes a void pointer `arg`, which is expected
 * to be the address of a null-terminated string. It casts this
 * argument back to a `char *` and prints the string to standard
 * output, followed by a newline character.
 *
 * Return: The argument `arg` is returned. If called as a thread entry point,
 * this return value can be retrieved by pthread_join.
 */
void *thread_entry(void *arg)
{
    char *str_arg;

    if (arg == NULL)
    {
        /* Handle null argument if necessary, though problem implies valid string */
        /* For this specific task, problem example passes a string */
        /* printf("(nil)\n"); */
        /* return (NULL); */
    }

    str_arg = (char *)arg;
    printf("%s\n", str_arg);

    return (arg); /* Or return (NULL); if arg is not meant to be joined/retrieved */
}