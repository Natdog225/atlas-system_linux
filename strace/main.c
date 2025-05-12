#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h> // For user_regs_struct
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "syscalls.h"

int main(int argc, char *argv[], char *envp[])
{
	pid_t child_pid;
	int status;
	struct user_regs_struct regs;
	int in_syscall = 0; // Flag to track if we are at syscall entry (0) or exit (1)

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
		return (EXIT_FAILURE);
	}

	child_pid = fork();
	if (child_pid == -1)
	{
		perror("fork");
		return (EXIT_FAILURE);
	}

	if (child_pid == 0)
	{ // Child process
		// Allow parent to trace
		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
			perror("ptrace(TRACEME)");
			exit(EXIT_FAILURE);
		}
		// Stop self to allow parent to set options or catch up
		// before execve potentially replaces the process image
		kill(getpid(), SIGSTOP);

		// Execute the target command
		// Note: argv+1 skips our program name, passing the command and its args
		if (execve(argv[1], argv + 1, envp) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE); // Exit if execve fails
		}
		// execve only returns on error
	}
	else
	{ // Parent process
		// Wait for the initial SIGSTOP from the child
		waitpid(child_pid, &status, 0);
		if (WIFEXITED(status))
			return (0); // Child exited early?

		// Main tracing loop
		while (1)
		{
			// Tell the child to continue until the next syscall entry or exit
			if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) == -1)
			{
				if (errno == ESRCH)
					break; // Child already exited
				perror("ptrace(SYSCALL)");
				return (EXIT_FAILURE);
			}

			// Wait for the child to stop (at syscall entry or exit)
			waitpid(child_pid, &status, 0);

			// Check if child exited normally
			if (WIFEXITED(status))
			{
				// Task 2/3: Need special handling for the last syscall (exit/exit_group)
				// because it doesn't return in the usual way.
				// We might print the exit status here if needed, or handle it
				// just before the loop breaks based on the last syscall seen.
				break;
			}

			// Check if child stopped due to a signal (like SIGTRAP for syscalls)
			if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
			{
				// Get registers at the stopping point
				if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) == -1)
				{
					perror("ptrace(GETREGS)");
					return (EXIT_FAILURE);
				}

				if (in_syscall == 0)
				{ // Syscall Entry
					// === TASK SPECIFIC CODE FOR SYSCALL ENTRY ===
					// Task 0: printf("%lld\n", regs.orig_rax);
					// Task 1: Print name based on regs.orig_rax
					// Task 2: Print name based on regs.orig_rax
					// Task 3: Print name and params (rdi, rsi, rdx, r10, r8, r9)

					in_syscall = 1; // Mark that we are now inside (waiting for exit)
				}
				else
				{ // Syscall Exit
					// === TASK SPECIFIC CODE FOR SYSCALL EXIT ===
					// Task 0: Nothing to print here
					// Task 1: Nothing to print here
					// Task 2: Print return value (regs.rax) in hex, handle exit_group
					// Task 3: Print return value (regs.rax) in hex, handle exit_group

					// Special handling for exit_group (syscall 231) or exit (syscall 60)
					if (regs.orig_rax == 231 || regs.orig_rax == 60)
					{
						// Handle the '?' for exit return value (Task 2/3)
						// Print final part of the line for Task 2/3
						fprintf(stdout, " = ?\n");
						fflush(stdout);
						break; // Exit the loop as the process is terminating
					}
					else
					{
						// Print final part of the line for Task 2/3
						// Example for Task 3: fprintf(stdout, " = %#llx\n", regs.rax); fflush(stdout);
						// Example for Task 2: fprintf(stdout, " = %#llx\n", regs.rax); fflush(stdout);
					}

					in_syscall = 0; // Ready for the next syscall entry
				}
			}
			else if (WIFSTOPPED(status))
			{
				// Stopped by another signal, could optionally report it
				// For this project, likely ignore or just continue
			}
		}
		// Cleanup or final print if needed (like exit status)
	}
	return (0);
}