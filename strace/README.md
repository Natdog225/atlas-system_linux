# Atlas System Linux - strace Project

This project involves building a simplified Linux `strace`-like utility using the `ptrace` system call in C.

## Requirements

* Editors: `vi`, `vim`, `emacs`
* Compiler: `gcc 9.4.0` on Ubuntu 20.04 LTS
* Flags: `-Wall -Werror -Wextra -pedantic`
* Style: Betty style (`betty-style.pl`, `betty-doc.pl`)
* No global variables
* Max 5 functions per file
* Standard library allowed
* Include guarded header files

## Tasks

### 0. Step #0 - Syscall number (`strace_0`)

* Prints the syscall number for each intercepted syscall.
* Usage: `./strace_0 <full_path_command> [args...]`

### 1. Step #1 - Syscall name (`strace_1`)

* Prints the syscall name for each intercepted syscall.
* Requires mapping syscall numbers (from `<asm/unistd_64.h>`) to names. A helper header `syscalls_64.h` is used for this.
* Usage: `./strace_1 <full_path_command> [args...]`

### 2. Step #2 - Return value (`strace_2`)

* Prints the syscall name and its return value in hexadecimal (`0x...`).
* Handles the `exit_group` syscall specially, printing `?` as its return value.
* Usage: `./strace_2 <full_path_command> [args...]`

### 3. Step #3 - Raw parameters (`strace_3`)

* Prints the syscall name, its parameters (up to 6) in hexadecimal, and its return value in hexadecimal.
* Requires knowledge of the x86_64 syscall calling convention (registers `rdi`, `rsi`, `rdx`, `r10`, `r8`, `r9`).
* Handles variadic syscalls (like `ioctl`) by printing `...` after the known parameters.
* Usage: `./strace_3 <full_path_command> [args...]`

## Compilation

Use the provided `Makefile`:
```bash
make             # Builds all targets
make strace_0    # Builds only step 0
make strace_1    # Builds only step 1
make strace_2    # Builds only step 2
make strace_3    # Builds only step 3
make clean       # Removes compiled binaries
make betty       # Runs Betty style checks