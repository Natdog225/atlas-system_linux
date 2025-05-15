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
 * find_syscall_entry - Finds syscall definition.
 * @syscall_num: Syscall number.
 * Return: Pointer to syscall_t entry, or NULL.
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
 * dump_write_buffer - Reads and prints child's write buffer content.
 * @pid: Child's PID.
 * @addr: Buffer address in child's memory.
 * @len: Buffer length to read.
 */
static void dump_write_buffer(pid_t pid, unsigned long addr, size_t len)
{
	size_t i, j;
	union {
		long val;
		char bytes[sizeof(long)];
	} word;
	char c;

	if (len > DUMP_MAX)
		len = DUMP_MAX;

	for (i = 0; i < len; i += sizeof(long))
	{
		errno = 0;
		word.val = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
		if (errno)
			break; /* Error reading memory */

		for (j = 0; j < sizeof(long) && i + j < len; j++)
		{
			c = word.bytes[j];
			if (c >= 32 && c < 127) /* Printable ASCII */
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
}


/**
 * parent_process - Traces child and prints syscalls.
 * @child: PID of the child process.
 * Return: 0 on success, 1 on error.
 */
int parent_process(pid_t child)
{
	int status;
	struct user_regs_struct regs;
	int is_syscall_entry = 1;
	unsigned long long current_syscall_nr = 0;
	const syscall_t *syscall_info = NULL;
	int seen_execve_exit = 0; /* Start tracing after execve completes */

	waitpid(child, &status, 0); /* Wait for initial SIGSTOP */
	if (WIFEXITED(status))
		return (0);

	if (ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD) == -1)
		perror("ptrace(PTRACE_SETOPTIONS)"); /* Continue if non-fatal */

	while (1)
	{
		if (ptrace(PTRACE_SYSCALL, child, 0, 0) == -1)
		{
			if (errno == ESRCH) break; /* Child exited */
			perror("ptrace(PTRACE_SYSCALL)"); return (1);
		}
		waitpid(child, &status, 0);

		if (WIFEXITED(status))
			break;

		/* Check for PTRACE_O_TRACESYSGOOD syscall stop */
		if (!(WIFSTOPPED(status) && (WSTOPSIG(status) & 0x80)))
			continue;

		if (ptrace(PTRACE_GETREGS, child, 0, &regs) == -1)
		{
			perror("ptrace(PTRACE_GETREGS)"); return (1);
		}

		if (is_syscall_entry) /* Syscall Entry */
		{
			current_syscall_nr = regs.orig_rax;
			syscall_info = find_syscall_entry(current_syscall_nr);

			/* Ignore syscalls before execve exit */
			if (!seen_execve_exit && current_syscall_nr != 59) /* SYS_execve */
			{
				continue;
			}

			if (current_syscall_nr == 60 || current_syscall_nr == 231) /* exit or exit_group */
			{
				if (syscall_info) fprintf(stdout, "%s = ?\n", syscall_info->name);
				else fprintf(stdout, "syscall_%llu = ?\n", current_syscall_nr);
				fflush(stdout);
				/* Loop will break on WIFEXITED after this */
			}
			else
			{
				is_syscall_entry = 0; /* Expecting syscall exit */
			}
		}
		else /* Syscall Exit */
		{
			/* Handle exit of initial execve */
			if (!seen_execve_exit)
			{
				if (current_syscall_nr == 59) /* SYS_execve */
				{
					seen_execve_exit = 1; /* Start "normal" tracing */
					if (syscall_info) fprintf(stdout, "%s = %#llx\n", syscall_info->name, (unsigned long long)regs.rax);
					else fprintf(stdout, "syscall_%llu = %#llx\n", current_syscall_nr, (unsigned long long)regs.rax);
					fflush(stdout);
				}
				is_syscall_entry = 1; /* Back to expecting entry */
				continue;
			}

			/* Skip ioctl */
			if (current_syscall_nr == 16) /* SYS_ioctl */
			{
				is_syscall_entry = 1; /* Reset for next syscall */
				continue;
			}

			char name_buffer[64];
			const char *name_to_print;

			if (syscall_info) name_to_print = syscall_info->name;
			else
			{
				sprintf(name_buffer, "syscall_%llu", current_syscall_nr);
				name_to_print = name_buffer;
			}

			fprintf(stdout, "%s", name_to_print); /* Print name */

			if (current_syscall_nr == 1) /* SYS_write */
			{
				if ((long long)regs.rax > 0) /* Successful write */
					dump_write_buffer(child, regs.rsi, (size_t)regs.rax);
				fprintf(stdout, " = %#llx\n", (unsigned long long)regs.rax);
			}
			else
			{
				fprintf(stdout, " = %#llx\n", (unsigned long long)regs.rax);
			}
			fflush(stdout);
			is_syscall_entry = 1; /* Expecting new syscall entry next */
		}
	}
	return (0);
}

/**
 * main - Entry point.
 * @argc: Argument count.
 * @argv: Argument vector.
 * @envp: Environment variables.
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
			perror("open /dev/null"); exit(EXIT_FAILURE);
		}
		if (dup2(devnull_fd, STDOUT_FILENO) == -1)
		{
			perror("dup2 to /dev/null"); exit(EXIT_FAILURE);
		}
		close(devnull_fd);

		if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
		{
			perror("ptrace(TRACEME)"); exit(EXIT_FAILURE);
		}
		/* Stop self for parent to attach and set options */
		if (raise(SIGSTOP) != 0)
		{
			perror("raise(SIGSTOP)"); exit(EXIT_FAILURE);
		}
		/* Execute command */
		execve(argv[1], argv + 1, envp);
		perror("execve"); /* execve only returns on error */
		exit(EXIT_FAILURE);
	}

	/* Parent process logic */
	return (parent_process(child));
}
