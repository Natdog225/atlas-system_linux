#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <errno.h>
#include <signal.h>

/**
 * main - Executes and traces a command, printing syscall numbers on entry.
 * @argc: The number of command-line arguments.
 * @argv: The array of command-line arguments.
 * @envp: The array of environment variables.
 *
 * Return: EXIT_SUCCESS on successful completion, EXIT_FAILURE on error.
 */
int main(int argc, char *argv[], char *envp[]) {
    pid_t child_pid;
    int status;
    struct user_regs_struct regs;
    int syscall_entry = 1; /* True for first entry */

    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
        return (EXIT_FAILURE);
    }

    child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        return (EXIT_FAILURE);
    }

    if (child_pid == 0) { /* Child process */
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            perror("ptrace(TRACEME)");
            exit(EXIT_FAILURE);
        }
        if (kill(getpid(), SIGSTOP) == -1) {
            perror("kill(SIGSTOP)");
            exit(EXIT_FAILURE);
        }
        if (execve(argv[1], argv + 1, envp) == -1) {
            perror("execve");
            exit(EXIT_FAILURE);
        }
    } else { /* Parent process */
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status)) {
            return (EXIT_SUCCESS);
        }

        while (1) {
            if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) == -1) {
                if (errno == ESRCH) {
                    break;
                }
                perror("ptrace(SYSCALL) continue");
                return (EXIT_FAILURE);
            }
            waitpid(child_pid, &status, 0);

            if (WIFEXITED(status)) {
                break;
            }

            if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
                if (syscall_entry) {
                    if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) == -1) {
                        perror("ptrace(GETREGS)");
                        return (EXIT_FAILURE);
                    }
                    fprintf(stdout, "%lld\n", regs.orig_rax);
                    fflush(stdout);
                }
                syscall_entry = !syscall_entry;
            }
        }
    }
    return (EXIT_SUCCESS);
}
