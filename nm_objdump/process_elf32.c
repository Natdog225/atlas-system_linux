#include "hnm.h"

int process_elf32(int fd, const char *filename, const char *prog_name,
				  bool swap_endian)
{
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdr_table = NULL;
	Elf32_Sym *symtab = NULL;
	char *shstrtab = NULL;
	char *strtab = NULL;
	int status = EXIT_FAILURE; /* Assume failure initially, like life */
	ssize_t bytes_read;
	uint16_t i; /* Use appropriate type for loop counter, uint16_t for shnum */
	Elf32_Shdr *symtab_shdr = NULL;
	Elf32_Shdr *strtab_shdr = NULL;
	size_t sh_tbl_size;
	size_t symtab_size, strtab_size, num_symbols;

	/* 1. Read ELF Header */
	bytes_read = read(fd, &ehdr, sizeof(ehdr));
	if (bytes_read == -1 || (size_t)bytes_read < sizeof(ehdr))
	{
		perror("read elf header");
		goto cleanup;
	}
	/* Basic validation already done */
	if (swap_endian)
		swap_endian_elf32_ehdr(&ehdr);

	/* 2. Read Section Header Table */
	sh_tbl_size = (size_t)ehdr.e_shnum * ehdr.e_shentsize;
	if (ehdr.e_shoff == 0 || ehdr.e_shnum == 0 || ehdr.e_shentsize == 0)
	{
		print_error(prog_name, filename, "No sections");
		status = EXIT_SUCCESS;
		goto cleanup;
	}
	shdr_table = malloc(sh_tbl_size);
	if (!shdr_table)
	{
		perror("malloc section headers");
		goto cleanup;
	}
	if (lseek(fd, ehdr.e_shoff, SEEK_SET) == -1)
	{
		perror("lseek to section headers");
		goto cleanup;
	}
	bytes_read = read(fd, shdr_table, sh_tbl_size);
	if (bytes_read == -1 || (size_t)bytes_read < sh_tbl_size)
	{
		perror("read section headers");
		goto cleanup;
	}
	/* Swap section headers if you want */
	if (swap_endian)
	{
		for (i = 0; i < ehdr.e_shnum; i++)
			swap_endian_elf32_shdr(&shdr_table[i]);
	}

	/* 3. Read Section Header String Table */
	if (ehdr.e_shstrndx != SHN_UNDEF && ehdr.e_shstrndx < ehdr.e_shnum)
	{
		Elf32_Shdr *shstrtab_shdr = &shdr_table[ehdr.e_shstrndx];
		shstrtab = malloc(shstrtab_shdr->sh_size);
		if (!shstrtab)
		{
			perror("malloc shstrtab");
			goto cleanup;
		}
		if (lseek(fd, shstrtab_shdr->sh_offset, SEEK_SET) == -1)
		{
			perror("lseek to shstrtab");
			goto cleanup;
		}
		bytes_read = read(fd, shstrtab, shstrtab_shdr->sh_size);
		if (bytes_read == -1 || (size_t)bytes_read < shstrtab_shdr->sh_size)
		{
			perror("read shstrtab");
			goto cleanup;
		}
	}

	/* 4. Find Symbol Table */
	for (i = 0; i < ehdr.e_shnum; i++)
	{
		if (shdr_table[i].sh_type == SHT_SYMTAB)
		{
			symtab_shdr = &shdr_table[i];
			if (symtab_shdr->sh_link >= ehdr.e_shnum) /* Check link validity */
			{
				print_error(prog_name, filename, "Invalid string table link");
				goto cleanup;
			}
			strtab_shdr = &shdr_table[symtab_shdr->sh_link];
			break; /* Process the first symbol table found */
		}
	}
	if (!symtab_shdr)
	{
		fprintf(stderr, "%s: %s: no symbols\n", prog_name, filename);
		status = EXIT_SUCCESS;
		goto cleanup;
	}

	/* 5. Read String Table (.strtab) */
	strtab_size = strtab_shdr->sh_size;
	strtab = malloc(strtab_size);
	if (!strtab)
	{
		perror("malloc strtab");
		goto cleanup;
	}
	if (lseek(fd, strtab_shdr->sh_offset, SEEK_SET) == -1)
	{
		perror("lseek to strtab");
		goto cleanup;
	}
	bytes_read = read(fd, strtab, strtab_size);
	if (bytes_read == -1 || (size_t)bytes_read < strtab_size)
	{
		perror("read strtab");
		goto cleanup;
	}

	/* 6. Read Symbol Table (.symtab) */
	symtab_size = symtab_shdr->sh_size;
	if (symtab_shdr->sh_entsize == 0) /* Avoid division by zero, insert meme */
	{
		print_error(prog_name, filename, "Invalid symbol table entry size");
		goto cleanup;
	}
	num_symbols = symtab_size / symtab_shdr->sh_entsize;
	symtab = malloc(symtab_size);
	if (!symtab)
	{
		perror("malloc symtab");
		goto cleanup;
	}
	if (lseek(fd, symtab_shdr->sh_offset, SEEK_SET) == -1)
	{
		perror("lseek to symtab");
		goto cleanup;
	}
	bytes_read = read(fd, symtab, symtab_size);
	if (bytes_read == -1 || (size_t)bytes_read < symtab_size)
	{
		perror("read symtab");
		goto cleanup;
	}

	/* Swap symbols if you want */
	if (swap_endian)
	{
		for (i = 0; i < num_symbols; i++)
			swap_endian_elf32_sym(&symtab[i]);
	}

	/* 7. Process and Print Symbols */
	for (i = 0; i < num_symbols; i++)
	{
		Elf32_Sym current_sym = symtab[i];
		char sym_type;
		const char *sym_name = "";

		/* Skip symbols without names (like bob) */
		if (current_sym.st_name != 0)
		{
			/* check bounds? */
			if (current_sym.st_name < strtab_size)
				sym_name = strtab + current_sym.st_name;
			else
				sym_name = "INVALID_NAME_OFFSET";

			sym_type = get_symbol_type32(current_sym, shdr_table, swap_endian);

			/* Filter out types we don't want (like '?') */
			if (sym_type == 'U' || sym_type == 'w')
			{
				/* Undefined or weak undefined - print no value */
				printf("         %c %s\n", sym_type, sym_name);
			}
			else
			{
				printf("%08x %c %s\n",
					   (unsigned int)current_sym.st_value,
					   sym_type, sym_name);
			}
		}
	}

status = EXIT_SUCCESS; /* If we reached here we did it! */

cleanup :
	/* 8. Free allocated memory */
	free(shstrtab);
	free(shdr_table);
	free(strtab);
	free(symtab);
	return (status);
}