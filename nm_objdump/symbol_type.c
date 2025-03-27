#include "hnm.h"

/**
 * get_symbol_type64 - Determines the nm type character for a 64-bit symbol.
 * @sym: The 64-bit symbol table entry.
 * @shdr_table: Pointer to the array of 64-bit section headers.
 * @swap_endian: Flag indicating if section headers were swapped
 * Return: The character representing the symbol type, or '?' if unknown.
 */
char get_symbol_type64(Elf64_Sym sym, Elf64_Shdr *shdr_table, bool swap_endian)
{
	char c = '?';
	unsigned char bind = ELF64_ST_BIND(sym.st_info);
	unsigned char type = ELF64_ST_TYPE(sym.st_info);
	Elf64_Shdr *shdr = NULL;

	(void)swap_endian;

	/* 1. Handle special section indices first */
	if (sym.st_shndx == SHN_UNDEF) c = 'U';
	else if (sym.st_shndx == SHN_ABS) c = 'A';
	else if (sym.st_shndx == SHN_COMMON) c = 'C';
	else if (sym.st_shndx < SHN_LORESERVE)
	{
		shdr = &shdr_table[sym.st_shndx];

		/* 2. Determine type based section flags */
		if ((shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_EXECINSTR)) {
			c = 'T';
		} else if (shdr->sh_type == SHT_NOBITS &&
			   (shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_WRITE)) {
			c = 'B';
		} else if ((shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_WRITE)) {
			if (shdr->sh_type == SHT_DYNAMIC) c = 'D';
			else c = 'D';
		} else if (shdr->sh_flags & SHF_ALLOC) {
			c = 'R';
		} else if (shdr->sh_type == SHT_NOTE) {
			c = 'n';
		}
	}

	/* 3. Adjust case based on binding */
	if (bind == STB_GNU_UNIQUE) c = 'u';
	else if (bind == STB_WEAK) {
		c = (sym.st_shndx == SHN_UNDEF) ? 'w' : 'W';
	} else if (bind == STB_LOCAL && c != '?') {
		c = tolower(c);
	}

	/* 4. Filter out specific symbol types */
	if (type == STT_FILE || type == STT_SECTION) c = '?';

	return (c);
}

/**
 * get_symbol_type32 - Determines the nm type character for a 32-bit
 * @sym: The 32-bit symbol table entry.
 * @shdr_table: Pointer to the array of 32-bit section headers.
 * @swap_endian: Flag indicating if section headers were swapped
 * Return: The character representing the symbol type, or '?' if unknown.
 */
char get_symbol_type32(Elf32_Sym sym, Elf32_Shdr *shdr_table, bool swap_endian)
{
	char c = '?';
	unsigned char bind = ELF32_ST_BIND(sym.st_info);
	unsigned char type = ELF32_ST_TYPE(sym.st_info);
	Elf32_Shdr *shdr = NULL;

	(void)swap_endian;

	/* 1. Handle special section indices first */
	if (sym.st_shndx == SHN_UNDEF) c = 'U';
	else if (sym.st_shndx == SHN_ABS) c = 'A';
	else if (sym.st_shndx == SHN_COMMON) c = 'C';
	else if (sym.st_shndx < SHN_LORESERVE)
	{
		shdr = &shdr_table[sym.st_shndx];

		/* 2. Determine type based section flags */
		if ((shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_EXECINSTR)) {
			c = 'T';
		} else if (shdr->sh_type == SHT_NOBITS &&
			   (shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_WRITE)) {
			c = 'B';
		} else if ((shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_WRITE)) {
			if (shdr->sh_type == SHT_DYNAMIC) c = 'D';
			else c = 'D';
		} else if (shdr->sh_flags & SHF_ALLOC) {
			c = 'R';
		} else if (shdr->sh_type == SHT_NOTE) {
			c = 'n';
		}
	}

	/* 3. Adjust case based on binding */
	if (bind == STB_GNU_UNIQUE) c = 'u';
	else if (bind == STB_WEAK) {
		c = (sym.st_shndx == SHN_UNDEF) ? 'w' : 'W';
	} else if (bind == STB_LOCAL && c != '?') {
		c = tolower(c);
	}

	/* 4. Filter out specific symbol types */
	if (type == STT_FILE || type == STT_SECTION) c = '?';

	return (c);
}
