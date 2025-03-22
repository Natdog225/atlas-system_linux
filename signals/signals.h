#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>
#include <sys/types.h>


int handle_signal(void);

void (*current_handler_signal(void))(int);

int handle_sigaction(void);

void (*current_handler_sigaction(void))(int);

int trace_signal_sender(void);

int pid_exist(pid_t pid);

#endif /* SIGNALS_H */
