#!/usr/bin/python3

"""
python

replace_string_in_heap.py: A Python script to find and replace a
    string in the heap memory of a running process using ptrace and ctypes.

Usage:
    python replace_string_in_heap.py <pid>

Arguments:
    <pid>          The process ID of the target process.

Example:
    python replace_string_in_heap.py 1234

Modules:
    os      The os module provides a way of using operating
            system-dependent functionality.
    sys     The sys module provides access to some variables used or
            maintained by the interpreter.
    ctypes  The ctypes module provides C compatible data types
            and allows calling functions in DLLs or shared libraries.

"""

import os
import sys
import ctypes

# ptrace will send a specific trace using one of these
PTRACE_ATTACH = 16
PTRACE_DETACH = 17

libc = ctypes.CDLL("libc.so.6")


# pid will thread ID of the corresponding Linux thread
def attach(pid):
    # print(f"Attaching to PID {pid}")
    if libc.ptrace(PTRACE_ATTACH, pid, None, None) == -1:
        sys.exit(1)
    os.waitpid(pid, 0)
    # print("Attached successfully")


def detach(pid):
    # print(f"Detaching from PID {pid}")
    if libc.ptrace(PTRACE_DETACH, pid, None, None) == -1:
        sys.exit(1)


def find_and_replace_string(pid, search_string, replace_string):
    mem_path = f"/proc/{pid}/mem"
    maps_path = f"/proc/{pid}/maps"

    heap_found = False
    with open(maps_path, "r") as maps_file:
        for line in maps_file:
            # print(line.strip())
            if "heap" in line:
                addr_range = line.split()[0]
                start, end = [int(x, 16) for x in addr_range.split("-")]
                heap_found = True
                break

    if not heap_found:
        print("Error: Heap not found")
        sys.exit(1)

    with open(mem_path, "rb+") as mem_file:
        mem_file.seek(start)
        heap_data = mem_file.read(end - start)

        offset = heap_data.find(search_string.encode())
        if offset == -1:
            print(f"'{search_string}' not found in heap")
            return

        mem_file.seek(start + offset)
        mem_file.write(replace_string.encode()
                       + b'\x00' * (len(search_string) - len(replace_string)))


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: read_write_heap.py pid search_string replace_string")
        sys.exit(1)

    pid = int(sys.argv[1])
    search_string = sys.argv[2]
    replace_string = sys.argv[3]

    if not search_string.isascii() or not replace_string.isascii():
        print("Error: Strings must be ascii")
        sys.exit(1)
    try:
        attach(pid)
        find_and_replace_string(pid, search_string, replace_string)
    finally:
        detach(pid)