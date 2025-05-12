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

	for (i = 0; syscalls_64_g[i].name; i++) /* Loop while name is not NULL */
	{
		if (syscalls_64_g[i].nr == (size_t)syscall_num)
			return (&syscalls_64_g[i]);
	}
	return (NULL);
}

/**
 * main - Executes and traces a command, printing syscall names.
 * Includes workaround for specific execve ptrace behavior.
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
	int syscall_entry_flag = 1; /* For alternating prints, start true */

	/* Flags for execve workaround (assuming 3 SIGTRAPs for execve in this env) */
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
			exit(EXIT_FAILURE); /* perror done by parent if needed */
		if (kill(getpid(), SIGSTOP) == -1)
			exit(EXIT_FAILURE);
		if (execve(argv[1], argv + 1, envp) == -1)
			exit(EXIT_FAILURE); /* perror done by parent */
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
					if (execve_trap_count == 1) /* Print only on the 1st trap */
					{
						if (entry) fprintf(stdout, "%s\n", entry->name);
						else fprintf(stdout, "syscall_%ld\n", syscall_num);
						fflush(stdout);
					}
					/* Manage flag for the 3-trap execve sequence */
					if (execve_trap_count == 1) syscall_entry_flag = 0;
					else if (execve_trap_count == 2) syscall_entry_flag = 1;
					else if (execve_trap_count >= 3) /* End of execve phase */
					{
						in_execve_startup_phase = 0;
						syscall_entry_flag = 1; /* Ensure next distinct syscall prints */
					}
				}
				else /* Not initial execve, or phase is over */
				{
					/* If we just exited execve phase, ensure flag is correct */
					if (in_execve_startup_phase)
					{
						in_execve_startup_phase = 0;
						syscall_entry_flag = 1;
					}

					if (syscall_num == 231) /* exit_group: always print */
					{
						if (entry) fprintf(stdout, "%s\n", entry->name);
						else fprintf(stdout, "syscall_%ld\n", syscall_num);
						fflush(stdout);
						/* Loop will break on next WIFEXITED */
					}
					else if (syscall_entry_flag) /* Regular alternating print */
					{
						if (entry) fprintf(stdout, "%s\n", entry->name);
						else fprintf(stdout, "syscall_%ld\n", syscall_num);
						fflush(stdout);
						syscall_entry_flag = !syscall_entry_flag;
					}
					else /* Not printing this one, just toggle */
					{
						syscall_entry_flag = !syscall_entry_flag;
					}
				}
			}
		}
	}
	return (EXIT_SUCCESS);
}
