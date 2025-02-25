#ifndef ELF_UTILS_H
#define ELF_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

/**
 * @brief Union representing both 32-bit and 64-bit ELF headers
 */
typedef union Elf_Ehdr
{
	Elf64_Ehdr *ehdr64;
	Elf32_Ehdr *ehdr32;
} Elf_Ehdr;

const char *get_osabi_string(unsigned char osabi, char *buffer, size_t buf_size);
const char *get_elf_type_string(unsigned short type);
const char *get_machine_string(unsigned short machine);
uint16_t read16(const void *data, int big_endian);
uint32_t read32(const void *data, int big_endian);

#endif