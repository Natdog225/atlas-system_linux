/* Include necessary headers */
#include <stdio.h>	   /* For fprintf, stderr, stdout, fflush */
#include <stdlib.h>	   /* For EXIT_FAILURE, EXIT_SUCCESS, exit */
#include <unistd.h>	   /* For fork, execve, getpid */
#include <sys/ptrace.h>/* For ptrace */
#include <sys/wait.h>  /* For waitpid, WIFEXITED, WIFSTOPPED, WSTOPSIG */
#include <sys/user.h>  /* For user_regs_struct */
#include <errno.h>	   /* For errno */
#include <signal.h>	   /* For kill, SIGSTOP, SIGTRAP */
#include "syscalls.h"
#include <sys/regset.h>

/**
 * main - Executes and traces a command, printing syscall numbers on entry.
 * @argc: The number of command-line arguments.
 * @argv: The array of command-line arguments.
 * argv[0] is the program name ("./strace_0").
 * argv[1] is the full path to the command to trace.
 * argv[2..] are the arguments to the command.
 * @envp: The array of environment variables.
 *
 * Return: EXIT_SUCCESS on successful completion, EXIT_FAILURE on error.
 */
int main(int argc, char *argv[], char *envp[])
{
	pid_t child_pid;
	int status;
	struct user_regs_struct regs;
	/* Flag to alternate between syscall entry (print) and exit (don't print) */
	int syscall_entry = 1;

	/* Validate command-line arguments */
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (EXIT_FAILURE);
	}

	/* Create a child process */
	child_pid = fork();
	if (child_pid == -1) /* Handle fork error */
	{
		perror("fork");
		return (EXIT_FAILURE);
	}

	if (child_pid == 0) /* === Child process (Tracee) === */
	{
		/* Allow the parent to trace this process */
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
			perror("ptrace(TRACEME)");
			exit(EXIT_FAILURE); /* Use exit in child, not return */
		}
		/* Stop self to ensure parent traces from the beginning (before execve) */
		if (kill(getpid(), SIGSTOP) == -1)
		{
			perror("kill(SIGSTOP)");
			exit(EXIT_FAILURE);
		}
		/* Execute the target command (argv[1]) with its arguments (argv + 1) */
		/* execve replaces the current process image */
		if (execve(argv[1], argv + 1, envp) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE); /* Exit if execve fails */
		}
		/* execve only returns on error */
	}
	else /* === Parent process (Tracer) === */
	{
		/* Wait for the child's initial SIGSTOP */
		waitpid(child_pid, &status, 0);
		if (WIFEXITED(status))
			return (EXIT_SUCCESS); /* Child exited unexpectedly */

		/* Main tracing loop: continue until the child exits */
		while (1)
		{
			/* Continue the child process until the next syscall entry or exit */
			if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) == -1)
			{
				if (errno == ESRCH) /* No such process: child has exited */
					break;
				perror("ptrace(SYSCALL) continue");
				return (EXIT_FAILURE);
			}
			/* Wait for the child to stop again */
			waitpid(child_pid, &status, 0);

			/* Check if the child process has exited normally */
			if (WIFEXITED(status))
				break;

			/* Check if stopped because of a syscall (SIGTRAP is used) */
			if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
			{
				/* Only print information on syscall entry */
				if (syscall_entry)
				{
					/* Retrieve the child's registers */
					if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) == -1)
					{
						perror("ptrace(GETREGS)");
						return (EXIT_FAILURE);
					}
					/* The syscall number on x86_64 is in orig_rax */
					fprintf(stdout, "%lld\n", regs.orig_rax);
					/* Ensure the output is immediately visible */
					fflush(stdout);
				}
				/* Toggle the flag for the next stop (entry/exit alternation) */
				syscall_entry = !syscall_entry;
			}
		}
	}
	/* Program completed successfully */
	return (EXIT_SUCCESS);
}