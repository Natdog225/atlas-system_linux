#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>  /* For struct user_regs_struct */
#include <errno.h>
#include <signal.h>
#include "syscalls.h"  /* For syscall_t and syscalls_64_g array */

/**
 * find_syscall_entry - Finds the syscall definition by its number.
 * @syscall_num: The number of the system call.
 *
 * Return: A pointer to the constant syscall_t entry if found, otherwise NULL.
 */
const syscall_t *find_syscall_entry(long syscall_num)
{
	size_t i;

	/* Iterate through the syscalls_64_g array from syscalls.h */
	for (i = 0; syscalls_64_g[i].name; i++) /* Loop while name is not NULL */
	{
		if (syscalls_64_g[i].nr == (size_t)syscall_num)
			return (&syscalls_64_g[i]);
	}
	return (NULL); /* Syscall number not found */
}

/**
 * print_name_conditionally - Prints syscall name to stderr.
 * Omits newline for SYS_write (syscall 1).
 * @syscall_num: The number of the system call.
 * @entry: Pointer to the syscall_t entry.
 */
void print_name_conditionally(long syscall_num, const syscall_t *entry)
{
	const char *name_to_print;
	char fallback_name[32]; /* Buffer for "syscall_XXX" */

	if (entry)
	{
		name_to_print = entry->name;
	}
	else
	{
		sprintf(fallback_name, "syscall_%ld", syscall_num);
		name_to_print = fallback_name;
	}

	fprintf(stderr, "%s", name_to_print); /* Print name to stderr */

	/* Add newline for all syscalls except SYS_write (1) */
	if (syscall_num != 1)
	{
		fprintf(stderr, "\n");
	}
	fflush(stderr); /* Flush stderr after printing */
}

/**
 * main - Executes and traces a command, printing syscall names to stderr.
 * @argc: The number of command-line arguments.
 * @argv: The array of command-line arguments.
 * @envp: The array of environment variables.
 *
 * Return: EXIT_SUCCESS on successful completion, EXIT_FAILURE on error.
 */
int main(int argc, char *argv[], char *envp[])
{
	pid_t child_pid;
	int status;
	struct user_regs_struct regs;
	int print_on_entry_flag = 1; /* For alternating prints */

	int in_execve_startup_phase = 1;
	int execve_trap_count = 0;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (EXIT_FAILURE);
	}

	child_pid = fork();
	if (child_pid == -1)
	{
		perror("fork");
		return (EXIT_FAILURE);
	}

	if (child_pid == 0) /* Child process */
	{
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
			exit(EXIT_FAILURE);
		if (kill(getpid(), SIGSTOP) == -1)
			exit(EXIT_FAILURE);
		if (execve(argv[1], argv + 1, envp) == -1)
			exit(EXIT_FAILURE);
	}
	else /* Parent process */
	{
		waitpid(child_pid, &status, 0); /* Wait for initial SIGSTOP */
		if (WIFEXITED(status))
			return (EXIT_SUCCESS);

		while (1)
		{
			if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) == -1)
			{
				if (errno == ESRCH) /* Child has exited */
					break;
				perror("ptrace(SYSCALL)"); return (EXIT_FAILURE);
			}
			waitpid(child_pid, &status, 0);

			if (WIFEXITED(status))
				break;

			if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
			{
				long syscall_num;
				const syscall_t *entry;

				if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) == -1)
				{
					perror("ptrace(GETREGS)"); return (EXIT_FAILURE);
				}
				syscall_num = regs.orig_rax;
				entry = find_syscall_entry(syscall_num);

				if (in_execve_startup_phase && syscall_num == 59) /* execve */
				{
					execve_trap_count++;
					if (execve_trap_count == 1) /* Print only on 1st trap */
					{
						print_name_conditionally(syscall_num, entry);
					}
					/* Manage flags for execve sequence for 3-trap */
					if (execve_trap_count >= 3)
					{
						in_execve_startup_phase = 0;
						print_on_entry_flag = 1; /* For next distinct syscall */
					}
					else
					{
						print_on_entry_flag = !print_on_entry_flag;
					}
				}
				else /* Normal syscall processing */
				{
					/* If we just exited execve phase (e.g. execve took <3 traps) */
					if (in_execve_startup_phase)
					{
						in_execve_startup_phase = 0;
						print_on_entry_flag = 1; /* Ensure this syscall prints */
					}

					if (syscall_num == 231) /* exit_group: always print */
					{
						print_name_conditionally(syscall_num, entry);
						/* Loop will break soon due to WIFEXITED */
					}
					else if (print_on_entry_flag) /* Regular alternating print */
					{
						print_name_conditionally(syscall_num, entry);
						print_on_entry_flag = !print_on_entry_flag;
					}
					else /* Not printing this one, just toggle for next entry */
					{
						print_on_entry_flag = !print_on_entry_flag;
					}
				}
			}
		}
	}
	return (EXIT_SUCCESS);
}
