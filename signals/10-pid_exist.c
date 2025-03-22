#include <signal.h>
#include <errno.h>

int pid_exist(pid_t pid)
{
	return (kill(pid, 0) == 0 || errno != ESRCH);
}