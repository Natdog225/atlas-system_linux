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
 * @syscall_num: The number of the system call (from regs.orig_rax).
 *
 * Return: A pointer to the constant syscall_t entry if found, otherwise NULL.
 * The syscalls_64_g array is assumed to be properly defined in
 * syscalls.h and accessible.
 */
const syscall_t *find_syscall_entry(long syscall_num)
{
	size_t i;

	/* Iterate through the syscalls_64_g array */
	for (i = 0; syscalls_64_g[i].name != NULL; i++)
	{
		if (syscalls_64_g[i].nr == (size_t)syscall_num)
		{
			return (&syscalls_64_g[i]);
		}
	}
	return (NULL); /* Syscall number not found */
}

/**
 * main - Executes and traces a command, printing syscall names on entry.
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
	int syscall_entry_flag = 1; /* True for first entry */

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
		{
			perror("ptrace(TRACEME)");
			exit(EXIT_FAILURE);
		}
		if (kill(getpid(), SIGSTOP) == -1)
		{
			perror("kill(SIGSTOP)");
			exit(EXIT_FAILURE);
		}
		if (execve(argv[1], argv + 1, envp) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
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
				perror("ptrace(SYSCALL) continue");
				return (EXIT_FAILURE);
			}
			waitpid(child_pid, &status, 0);

			if (WIFEXITED(status))
				break;

			if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
			{
				if (syscall_entry_flag)
				{
					long syscall_num;
					const syscall_t *entry;

					if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) == -1)
					{
						perror("ptrace(GETREGS)");
						return (EXIT_FAILURE);
					}
					syscall_num = regs.orig_rax;
					entry = find_syscall_entry(syscall_num);

					if (entry)
					{
						fprintf(stdout, "%s\n", entry->name);
					}
					else
					{
						/* Fallback if syscall number not in table */
						fprintf(stdout, "syscall_%ld\n", syscall_num);
					}
					fflush(stdout);
				}
				syscall_entry_flag = !syscall_entry_flag;
			}
		}
	}
	return (EXIT_SUCCESS);
}
