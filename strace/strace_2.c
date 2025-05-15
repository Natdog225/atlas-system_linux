#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include "syscalls.h"

#define DUMP_MAX 32 /* Max bytes to dump from write buffer */

/**
 * find_syscall_entry - Finds syscall definition by number.
 * @syscall_num: Syscall number.
 *
 * Return: Pointer to syscall_t entry, or NULL if not found.
 */
const syscall_t *find_syscall_entry(long syscall_num)
{
	size_t i;

	for (i = 0; syscalls_64_g[i].name; i++)
	{
		if (syscalls_64_g[i].nr == (size_t)syscall_num)
			return (&syscalls_64_g[i]);
	}
	return (NULL);
}

/**
 * dump_write_buffer - Reads and prints content from child's write buffer.
 * @pid: Child's process ID.
 * @addr: Buffer address in child's memory.
 * @len: Buffer length to read (from write syscall's return value).
 */
static void dump_write_buffer(pid_t pid, unsigned long addr, size_t len)
{
	size_t i, j;
	union
	{
		long val;
		char bytes[sizeof(long)];
	} word;
	char c;

	fprintf(stdout, "\""); /* Start quote */

	if (len > DUMP_MAX)
		len = DUMP_MAX;

	for (i = 0; i < len; i += sizeof(long))
	{
		errno = 0;
		word.val = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		if (errno)
		{
			fprintf(stdout, "..."); /* Error reading memory */
			break;
		}

		for (j = 0; j < sizeof(long) && i + j < len; j++)
		{
			c = word.bytes[j];
			/* Handle printable chars and common escapes */
			if (c >= 32 && c < 127)
				putchar(c);
			else if (c == '\n')
				printf("\\n");
			else if (c == '\t')
				printf("\\t");
			else if (c == '\r')
				printf("\\r");
			else
				printf("\\x%02x", (unsigned char)c);
		}
	}
	fprintf(stdout, "\""); /* End quote */
	if (len > DUMP_MAX)
		fprintf(stdout, "..."); /* Indicate truncation */
}

/**
 * parent_process - Traces child and prints syscalls.
 * @child: PID of the child process.
 *
 * Return: 0 on success, 1 on error.
 */
int parent_process(pid_t child)
{
	int status;
	struct user_regs_struct regs;
	int in_syscall_entry = 1; /* 1 for entry, 0 for exit */
	unsigned long long current_syscall_nr = 0;
	const syscall_t *syscall_info = NULL;

	waitpid(child, &status, 0); /* Initial SIGSTOP */
	if (WIFEXITED(status))
		return (0);

	if (ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD) == -1)
	{
		perror("ptrace(PTRACE_SETOPTIONS)");
	}

	while (1)
	{
		if (ptrace(PTRACE_SYSCALL, child, 0, 0) == -1)
		{
			if (errno == ESRCH)
				break;
			perror("ptrace(PTRACE_SYSCALL)");
			return (1);
		}
		waitpid(child, &status, 0);

		if (WIFEXITED(status))
			break;

		/* Ensure it's a PTRACE_O_TRACESYSGOOD syscall trap */
		if (!(WIFSTOPPED(status) && (WSTOPSIG(status) & 0x80)))
			continue;

		if (ptrace(PTRACE_GETREGS, child, 0, &regs) == -1)
		{
			perror("ptrace(PTRACE_GETREGS)");
			return (1);
		}

		if (in_syscall_entry) /* Syscall Entry */
		{
			current_syscall_nr = regs.orig_rax;
			syscall_info = find_syscall_entry(current_syscall_nr);

			if (current_syscall_nr == 231 || current_syscall_nr == 60) /* exit_group or exit */
			{
				if (syscall_info)
					fprintf(stdout, "%s(", syscall_info->name);
				else
					fprintf(stdout, "syscall_%llu(", current_syscall_nr);
				fprintf(stdout, "%lld) = ?\n", regs.rdi); /* Print first arg */
				fflush(stdout);
			}
			else
			{
				in_syscall_entry = 0; /* Expecting syscall exit next */
			}
		}
		else /* Syscall Exit */
		{
			char name_buffer[64];

			if (!syscall_info) /* Fallback name if unknown on entry */
			{
				sprintf(name_buffer, "syscall_%llu", current_syscall_nr);
			}

			fprintf(stdout, "%s(", syscall_info ? syscall_info->name : name_buffer);

			fprintf(stdout, ") = ");

			if (current_syscall_nr == 1) /* write syscall */
			{
				if ((long long)regs.rax > 0) /* Successful write */
				{
					dump_write_buffer(child, regs.rsi, (size_t)regs.rax);
				}
				fprintf(stdout, " %#llx\n", (unsigned long long)regs.rax);
			}
			else
			{
				fprintf(stdout, "%#llx\n", (unsigned long long)regs.rax);
			}
			fflush(stdout);
			in_syscall_entry = 1; /* Expecting new syscall entry next */
		}
	}
	return (0);
}

/**
 * main - Entry point. Forks, traces child, execs command.
 * @argc: Argument count.
 * @argv: Argument vector.
 * @envp: Environment variables.
 *
 * Return: 0 on success, 1 on error.
 */
int main(int argc, char *argv[], char **envp)
{
	pid_t child;
	int devnull_fd;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
		return (1);
	}

	child = fork();
	if (child == -1)
	{
		perror("fork");
		return (1);
	}

	if (child == 0) /* Child process */
	{
		/* Redirect child's stdout to /dev/null */
		devnull_fd = open("/dev/null", O_WRONLY);
		if (devnull_fd == -1)
		{
			perror("open /dev/null");
			exit(EXIT_FAILURE);
		}
		if (dup2(devnull_fd, STDOUT_FILENO) == -1)
		{
			perror("dup2 to /dev/null");
			exit(EXIT_FAILURE);
		}
		close(devnull_fd);

		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
			perror("ptrace(TRACEME)");
			exit(EXIT_FAILURE);
		}
		/* Stop self for parent to attach and set options */
		if (raise(SIGSTOP) != 0)
		{
			perror("raise(SIGSTOP)");
			exit(EXIT_FAILURE);
		}
		/* Execute command */
		execve(argv[1], argv + 1, envp);
		/* execve only returns on error */
		perror("execve");
		exit(EXIT_FAILURE);
	}

	/* Parent process logic */
	return (parent_process(child));
}
