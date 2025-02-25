#include <stdio.h>
#include <stdlib.h>
#include <elf.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
		return 1;
	}

	FILE *file = fopen(argv[1], "rb");
	if (!file)
	{
		perror("Error opening file");
		return 1;
	}

	Elf32_Ehdr ehdr; // Start with 32-bit structure
	fread(&ehdr, sizeof(Elf32_Ehdr), 1, file);

	// Check if it's a valid ELF file and determine 32/64-bit
	if (ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
		ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
		ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
		ehdr.e_ident[EI_MAG3] != ELFMAG3)
	{
		fprintf(stderr, "Error: Not an ELF file\n");
		fclose(file);
		return 1;
	}
	if (ehdr.e_ident[EI_CLASS] == ELFCLASS64)
	{
		fseek(file, 0, SEEK_SET);
		Elf64_Ehdr ehdr64;
		fread(&ehdr64, sizeof(Elf64_Ehdr), 1, file);
		printf("OS/ABI Value (64-bit): %u\n", ehdr64.e_ident[EI_OSABI]); // Test with %u
		printf("OS/ABI Value (64-bit, hex): 0x%x\n", ehdr64.e_ident[EI_OSABI]);
	}
	else
	{
		printf("OS/ABI Value (32-bit): %u\n", ehdr.e_ident[EI_OSABI]);		  // Test with %u
		printf("OS/ABI Value (32-bit, hex): 0x%x\n", ehdr.e_ident[EI_OSABI]); // Test with 0x%x
	}
	fclose(file);
	return 0;
}