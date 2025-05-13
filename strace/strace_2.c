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
 * main - Executes and traces a command, printing syscall names and return values.
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
	int is_syscall_entry = 1; /* 1 for entry, 0 for waiting for exit */

	/* Workaround for environments with multiple SIGTRAPs for initial execve */
	int initial_execve_phase = 1;
	int execve_trap_counter = 0;

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
				long syscall_num_curr; /* Syscall num from orig_rax */
				const syscall_t *syscall_info;

				if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) == -1)
				{
					perror("ptrace(GETREGS)"); return (EXIT_FAILURE);
				}
				syscall_num_curr = regs.orig_rax;
				syscall_info = find_syscall_entry(syscall_num_curr);

				if (initial_execve_phase && syscall_num_curr == 59) /* execve */
				{
					execve_trap_counter++;
					if (execve_trap_counter == 1) /* 1st execve trap (entry) */
					{
						if (syscall_info) fprintf(stdout, "%s", syscall_info->name);
						else fprintf(stdout, "syscall_%ld", syscall_num_curr);
						fflush(stdout);
						is_syscall_entry = 0; /* Now expect execve exit */
					}
					else if (execve_trap_counter == 2) /* 2nd execve trap (exit) */
					{
						fprintf(stdout, " = %#llx\n", regs.rax); /* Print return val */
						fflush(stdout);
						is_syscall_entry = 1; /* Next is a new syscall's entry */
					}
					else if (execve_trap_counter >= 3) /* 3rd+ execve trap (ignore) */
					{
						initial_execve_phase = 0; /* Done with special execve */
						is_syscall_entry = 1; /* Next distinct syscall is an entry */
					}
				}
				else /* Not initial execve, or special execve phase is over */
				{
					if (initial_execve_phase) /* First distinct syscall after execve */
					{
						initial_execve_phase = 0;
						is_syscall_entry = 1;   /* Ensure it's treated as an entry */
					}

					if (is_syscall_entry) /* Generic syscall entry */
					{
						if (syscall_info) fprintf(stdout, "%s", syscall_info->name);
						else fprintf(stdout, "syscall_%ld", syscall_num_curr);
						fflush(stdout);
						is_syscall_entry = 0; /* Next stop for this syscall is exit */
					}
					else /* Generic syscall exit */
					{
						if (syscall_num_curr == 231 || syscall_num_curr == 60) /* exit_group or exit */
						{
							fprintf(stdout, " = ?\n");
							fflush(stdout);
							break; /* Terminate tracing loop */
						}
						else
						{
							fprintf(stdout, " = %#llx\n", regs.rax);
							fflush(stdout);
						}
						is_syscall_entry = 1; /* Next stop is a new syscall's entry */
					}
				}
			}
		}
	}
	return (EXIT_SUCCESS);
}
