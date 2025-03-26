#include "hnm.h"

char get_symbol_type64(Elf64_Sym sym, Elf64_Shdr *shdr_table, bool swap_endian)
{
	char c = '?'; /* Default to unknown */
	unsigned char bind = ELF64_ST_BIND(sym.st_info);
	unsigned char type = ELF64_ST_TYPE(sym.st_info);
	Elf64_Shdr *shdr = NULL; /* Section header */

	(void)swap_endian;

	/* 1. Check special section indices */
	if (sym.st_shndx == SHN_UNDEF)
		c = 'U';
	else if (sym.st_shndx == SHN_ABS)
		c = 'A';
	else if (sym.st_shndx == SHN_COMMON)
		c = 'C';
	else if (sym.st_shndx < SHN_LORESERVE) /* Check if index is valid */
	{
		/* 2. Get the corresponding section header */
		shdr = &shdr_table[sym.st_shndx];

		/* 3. Determine type based on section properties */
		if (shdr->sh_type == SHT_NOBITS &&
			(shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_WRITE))
			c = 'B'; /* .bss */
		else if (shdr->sh_type == SHT_PROGBITS && (shdr->sh_flags & SHF_ALLOC) &&
				 (shdr->sh_flags & SHF_WRITE))
			c = 'D'; /* .data */
		else if (shdr->sh_type == SHT_PROGBITS && (shdr->sh_flags & SHF_ALLOC) &&
				 !(shdr->sh_flags & SHF_WRITE))
			c = 'R'; /* .rodata */
		else if ((shdr->sh_type == SHT_PROGBITS || shdr->sh_type == SHT_INIT_ARRAY ||
				  shdr->sh_type == SHT_FINI_ARRAY) &&
				 (shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_EXECINSTR))
			c = 'T'; /* .text */
		else if (shdr->sh_type == SHT_DYNAMIC)
			c = 'D'; /* Symbols in .dynamic section */
		/* Add more specific section checks if needed (e.g., SHT_NOTE -> 'n') */
		else if ((shdr->sh_flags & SHF_ALLOC) || (shdr->sh_flags & SHF_WRITE) ||
				 (shdr->sh_flags & SHF_EXECINSTR)) /* Check common alloc/write/exec flags */
		{
			if (shdr->sh_flags & SHF_EXECINSTR)
				c = 'T';
			else if (shdr->sh_flags & SHF_WRITE)
				c = 'D';
			else if (shdr->sh_flags & SHF_ALLOC)
				c = 'R';
		}
	}

	/* 4. Adjust based on Symbol Binding */
	if (bind == STB_GNU_UNIQUE)
	{
		c = 'u'; /* Always 'u' regardless of case */
	}
	else if (bind == STB_WEAK)
	{
		/* Weak symbols: 'W' or 'w'. */
		if (sym.st_shndx == SHN_UNDEF)
			c = 'w'; /* undefined */
		else
			c = 'W'; /* defined */
	}
	else if (bind == STB_LOCAL && c != '?')
	{
		c = tolower(c); /* Local symbols are lowercase */
	}

	/* 5. Handle STT_FILE (usually ignored by nm -p) */
	if (type == STT_FILE || type == STT_SECTION)
		c = '?'; /* filter out later */

	return (c);
}

char get_symbol_type32(Elf32_Sym sym, Elf32_Shdr *shdr_table, bool swap_endian)
{
	char c = '?'; /* Default to unknown */
	unsigned char bind = ELF32_ST_BIND(sym.st_info);
	unsigned char type = ELF32_ST_TYPE(sym.st_info);
	Elf32_Shdr *shdr = NULL;

	(void)swap_endian;

	/* 1. Check special section indices */
	if (sym.st_shndx == SHN_UNDEF)
		c = 'U';
	else if (sym.st_shndx == SHN_ABS)
		c = 'A';
	else if (sym.st_shndx == SHN_COMMON)
		c = 'C';
	else if (sym.st_shndx < SHN_LORESERVE)
	{
		/* 2. Get the corresponding section header */
		shdr = &shdr_table[sym.st_shndx];

		/* 3. Determine type based on section properties */
		if (shdr->sh_type == SHT_NOBITS &&
			(shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_WRITE))
			c = 'B'; /* .bss */
		else if (shdr->sh_type == SHT_PROGBITS && (shdr->sh_flags & SHF_ALLOC) &&
				 (shdr->sh_flags & SHF_WRITE))
			c = 'D'; /* .data */
		else if (shdr->sh_type == SHT_PROGBITS && (shdr->sh_flags & SHF_ALLOC) &&
				 !(shdr->sh_flags & SHF_WRITE))
			c = 'R'; /* .rodata */
		else if ((shdr->sh_type == SHT_PROGBITS || shdr->sh_type == SHT_INIT_ARRAY ||
				  shdr->sh_type == SHT_FINI_ARRAY) &&
				 (shdr->sh_flags & SHF_ALLOC) && (shdr->sh_flags & SHF_EXECINSTR))
			c = 'T'; /* .text */
		else if (shdr->sh_type == SHT_DYNAMIC)
			c = 'D'; /* Symbols in .dynamic section */
		else if ((shdr->sh_flags & SHF_ALLOC) || (shdr->sh_flags & SHF_WRITE) ||
				 (shdr->sh_flags & SHF_EXECINSTR))
		{
			if (shdr->sh_flags & SHF_EXECINSTR)
				c = 'T';
			else if (shdr->sh_flags & SHF_WRITE)
				c = 'D';
			else if (shdr->sh_flags & SHF_ALLOC)
				c = 'R';
		}
	}

	/* 4. Adjust based on Symbol Binding */
	if (bind == STB_GNU_UNIQUE)
	{
		c = 'u';
	}
	else if (bind == STB_WEAK)
	{
		if (sym.st_shndx == SHN_UNDEF)
			c = 'w';
		else
			c = 'W';
	}
	else if (bind == STB_LOCAL && c != '?')
	{
		c = tolower(c);
	}

	/* 5. Handle STT_FILE / STT_SECTION */
	if (type == STT_FILE || type == STT_SECTION)
		c = '?'; /* Mark to filter out later */

	return (c);
}
