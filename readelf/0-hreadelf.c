#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include "elf_utils.h" // Include the header file

/**
 * print_magic - Prints the magic number of the ELF header.
 * @magic: Pointer to the magic number bytes.
 */
void print_magic(unsigned char *magic)
{
	printf("  Magic:   %02x %02x %02x %02x %02x %02x %02x %02x "
		   "%02x %02x %02x %02x %02x %02x %02x %02x \n",
		   magic[0], magic[1], magic[2], magic[3], magic[4], magic[5],
		   magic[6], magic[7], magic[8], magic[9], magic[10], magic[11],
		   magic[12], magic[13], magic[14], magic[15]);
}
/**
 * print_data - Prints data of the ELF header.
 * @magic: Pointer to magic bytes
 * @osabi_buffer: Buffer for the osabi unknown cases.
 */
void print_data(unsigned char *magic, char *osabi_buffer)
{
	printf("  Class:                             %s\n",
		   magic[EI_CLASS] == ELFCLASS64 ? "ELF64" : "ELF32");
	printf("  Data:                              %s\n",
		   magic[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian"
										 : "2's complement, big endian");
	printf("  Version:                           %d (current)\n",
		   magic[EI_VERSION]);
	printf("  OS/ABI:                            %s\n",
		   get_osabi_string(magic[EI_OSABI], osabi_buffer, 32));
	printf("  ABI Version:                       %d\n",
		   magic[EI_ABIVERSION]);
}
/**
 * print_elf_header - Prints the complete ELF header information.
 * @ehdr: The ELF header (either 32-bit or 64-bit).
 * @is_64bit: Flag indicating if it's a 64-bit ELF.
 * @is_big_endian: Flag indicating if it's big-endian.
 */
void print_elf_header(union Elf_Ehdr ehdr, int is_64bit, int is_big_endian)
{
	uint16_t e_type = read16(is_64bit ? &ehdr.ehdr64->e_type : &ehdr.ehdr32->e_type, is_big_endian);
	uint16_t e_machine = read16(is_64bit ? &ehdr.ehdr64->e_machine : &ehdr.ehdr32->e_machine, is_big_endian);
	uint32_t e_version = read32(is_64bit ? (const void *)&ehdr.ehdr64->e_version : (const void *)&ehdr.ehdr32->e_version, is_big_endian);
	uint32_t e_entry = read32(is_64bit ? (const void *)&ehdr.ehdr64->e_entry : (const void *)&ehdr.ehdr32->e_entry, is_big_endian);
	uint32_t e_phoff = read32(is_64bit ? (const void *)&ehdr.ehdr64->e_phoff : (const void *)&ehdr.ehdr32->e_phoff, is_big_endian);
	uint32_t e_shoff = read32(is_64bit ? (const void *)&ehdr.ehdr64->e_shoff : (const void *)&ehdr.ehdr32->e_shoff, is_big_endian);
	uint32_t e_flags = read32(is_64bit ? (const void *)&ehdr.ehdr64->e_flags : (const void *)&ehdr.ehdr32->e_flags, is_big_endian);
	uint16_t e_ehsize = read16(is_64bit ? &ehdr.ehdr64->e_ehsize : &ehdr.ehdr32->e_ehsize, is_big_endian);
	uint16_t e_phentsize = read16(is_64bit ? &ehdr.ehdr64->e_phentsize : &ehdr.ehdr32->e_phentsize, is_big_endian);
	uint16_t e_phnum = read16(is_64bit ? &ehdr.ehdr64->e_phnum : &ehdr.ehdr32->e_phnum, is_big_endian);
	uint16_t e_shentsize = read16(is_64bit ? &ehdr.ehdr64->e_shentsize : &ehdr.ehdr32->e_shentsize, is_big_endian);
	uint16_t e_shnum = read16(is_64bit ? &ehdr.ehdr64->e_shnum : &ehdr.ehdr32->e_shnum, is_big_endian);
	uint16_t e_shstrndx = read16(is_64bit ? &ehdr.ehdr64->e_shstrndx : &ehdr.ehdr32->e_shstrndx, is_big_endian);

	printf("  Type:                              %s\n", get_elf_type_string(e_type));
	printf("  Machine:                           %s\n", get_machine_string(e_machine));
	printf("  Version:                           0x%x\n", e_version);
	printf("  Entry point address:               0x%x\n", e_entry);
	printf("  Start of program headers:          %u (bytes into file)\n", e_phoff);
	printf("  Start of section headers:          %u (bytes into file)\n", e_shoff);
	printf("  Flags:                             0x%x\n", e_flags);
	printf("  Size of this header:               %u (bytes)\n", e_ehsize);
	printf("  Size of program headers:           %u (bytes)\n", e_phentsize);
	printf("  Number of program headers:         %u\n", e_phnum);
	printf("  Size of section headers:           %u (bytes)\n", e_shentsize);
	printf("  Number of section headers:         %u\n", e_shnum);
	printf("  Section header string table index: %u\n", e_shstrndx);
}

/**
 * process_elf_header - Processes the ELF header and determines file type.
 * @file_data: Pointer to the mapped ELF file data.
 * @is_64bit: Pointer to store whether it's a 64-bit ELF.
 * @is_big_endian: Pointer to store whether it's big-endian.
 * @ehdr: Union to hold either 32-bit or 64-bit header.
 *
 * Return: 0 on success, 1 on error.
 */
int process_elf_header(void *file_data, int *is_64bit, int *is_big_endian, union Elf_Ehdr *ehdr)
{
	unsigned char *magic = (unsigned char *)file_data;

	if (magic[EI_MAG0] != ELFMAG0 || magic[EI_MAG1] != ELFMAG1 ||
		magic[EI_MAG2] != ELFMAG2 || magic[EI_MAG3] != ELFMAG3)
	{
		fprintf(stderr, "Error: Not an ELF file\n");
		return (1);
	}

	*is_big_endian = (magic[EI_DATA] == ELFDATA2MSB);
	*is_64bit = (magic[EI_CLASS] == ELFCLASS64);

	if (*is_64bit)
		ehdr->ehdr64 = (Elf64_Ehdr *)file_data;
	else
		ehdr->ehdr32 = (Elf32_Ehdr *)file_data;

	return (0);
}

/**
 * main - Entry point. Reads and displays ELF header information.
 * @argc: Argument count.
 * @argv: Argument vector.
 *
 * Return: 0 on success, 98 on error.
 */
int main(int argc, char *argv[])
{
	int fd, is_big_endian, is_64bit;
	struct stat st;
	void *file_data;
	char osabi_buffer[32];
	union Elf_Ehdr ehdr;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
		exit(98);
	}
	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		perror("Error: Can't open file");
		exit(98);
	}
	if (fstat(fd, &st) == -1)
	{
		perror("Error: Can't get file stats");
		close(fd);
		exit(98);
	}
	file_data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (file_data == MAP_FAILED)
	{
		perror("Error: mmap failed");
		close(fd);
		exit(98);
	}

	if (process_elf_header(file_data, &is_64bit, &is_big_endian, &ehdr) != 0)
	{
		munmap(file_data, st.st_size);
		close(fd);
		exit(98);
	}

	printf("ELF Header:\n");
	print_magic(file_data);
	print_data(file_data, osabi_buffer);
	print_elf_header(ehdr, is_64bit, is_big_endian);

	munmap(file_data, st.st_size);
	close(fd);
	return (0);
}