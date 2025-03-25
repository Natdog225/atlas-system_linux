/* hnm.h */
#ifndef HNM_H
#define HNM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <elf.h>
#include <ctype.h>
#include <stdint.h>


/* --- Function Prototypes --- */

/* Main processing functions */
int process_file(const char *filename, const char *prog_name);
int process_elf64(int fd, const char *filename, const char *prog_name, bool swap_endian);
int process_elf32(int fd, const char *filename, const char *prog_name, bool swap_endian);

/* Symbol type determination */
/* Note: Takes pointer to section header table */
char get_symbol_type64(Elf64_Sym sym, Elf64_Shdr *shdr_table, bool swap_endian);
char get_symbol_type32(Elf32_Sym sym, Elf32_Shdr *shdr_table, bool swap_endian);

/* Endian Swapping Utilities (64-bit) */
void swap_endian_elf64_ehdr(Elf64_Ehdr *ehdr);
void swap_endian_elf64_shdr(Elf64_Shdr *shdr);
void swap_endian_elf64_sym(Elf64_Sym *sym);

/* Endian Swapping Utilities (32-bit) */
void swap_endian_elf32_ehdr(Elf32_Ehdr *ehdr);
void swap_endian_elf32_shdr(Elf32_Shdr *shdr);
void swap_endian_elf32_sym(Elf32_Sym *sym);

/* Helper/Utility functions */
void print_error(const char *prog_name, const char *filename, const char *message);


#endif /* HNM_H */