#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

/* Function to get the OS/ABI string */
const char *get_osabi_string(unsigned char osabi, char *buffer, size_t buf_size)
{
	switch (osabi)
	{
	case ELFOSABI_SYSV:
		return "UNIX - System V";
	case ELFOSABI_HPUX:
		return "UNIX - HP-UX";
	case ELFOSABI_NETBSD:
		return "UNIX - NetBSD";
	case ELFOSABI_LINUX:
		return "UNIX - Linux";
	case ELFOSABI_SOLARIS:
		return "UNIX - Solaris";
	case ELFOSABI_IRIX:
		return "UNIX - IRIX";
	case ELFOSABI_FREEBSD:
		return "UNIX - FreeBSD";
	case ELFOSABI_TRU64:
		return "UNIX - TRU64";
	case ELFOSABI_ARM:
		return "ARM";
	case ELFOSABI_STANDALONE:
		return "Standalone App";
	default:
		/* Use the raw byte value directly */
		snprintf(buffer, buf_size, "<unknown: %u>", osabi);
		return buffer;
	}
}

/* Function to get the ELF type string */
const char *get_elf_type_string(unsigned short type)
{
	switch (type)
	{
	case ET_NONE:
		return "NONE (No file type)";
	case ET_REL:
		return "REL (Relocatable file)";
	case ET_EXEC:
		return "EXEC (Executable file)";
	case ET_DYN:
		return "DYN (Shared object file)";
	case ET_CORE:
		return "CORE (Core file)";
	default:
		return "<unknown>";
	}
}

/* Function to get the machine architecture string */
const char *get_machine_string(unsigned short machine)
{
	switch (machine)
	{
	case EM_NONE:
		return "None";
	case EM_M32:
		return "WE32100";
	case EM_SPARC:
		return "Sparc";
	case EM_386:
		return "Intel 80386";
	case EM_68K:
		return "MC68000";
	case EM_88K:
		return "MC88000";
	case EM_860:
		return "Intel 80860";
	case EM_MIPS:
		return "MIPS R3000";
	case EM_S370:
		return "IBM System/370";
	case EM_MIPS_RS3_LE:
		return "MIPS R4000 big-endian";
	case EM_PARISC:
		return "HPPA";
	case EM_SPARC32PLUS:
		return "Sparc v8+";
	case EM_960:
		return "Intel 80960";
	case EM_PPC:
		return "PowerPC";
	case EM_PPC64:
		return "PowerPC64";
	case EM_FR20:
		return "Fujitsu FR20";
	case EM_S390:
		return "IBM S/390";
	case EM_SPU:
		return "SPU";
	case EM_V800:
		return "Renesas V850";
	case EM_IA_64:
		return "Intel IA-64";
	case EM_X86_64:
		return "Advanced Micro Devices X86-64";
	case EM_VAX:
		return "Digital VAX";
	case EM_CRIS:
		return "Axis Comm. CRIS Architecture";
	case EM_JAVELIN:
		return "Infineon Tech. Javelin Processor";
	case EM_FIREPATH:
		return "Element 14 6500";
	case EM_ZSP:
		return "LSI Logic's 16-bit DSP Processor";
	case EM_MMIX:
		return "Donald Knuth's educational 64-bit proc";
	case EM_HUANY:
		return "Harvard University machine-independent object files";
	case EM_PRISM:
		return "SiTera Prism";
	case EM_ARM:
		return "Advanced RISC Machines ARM";
	case EM_SH:
		return "Renesas SuperH";
	case EM_SPARCV9:
		return "SPARC v9 64-bit";
	case EM_TRICORE:
		return "Siemens Tricore embedded processor";
	case EM_ARC:
		return "Argonaut RISC Core";
	case EM_H8_300:
		return "Renesas H8/300";
	case EM_H8_300H:
		return "Renesas H8/300H";
	case EM_H8S:
		return "Renesas H8S";
	case EM_H8_500:
		return "Renesas H8/500";
	case EM_AARCH64:
		return "ARM aarch64";
	case EM_TILEPRO:
		return "Tilera TILEPro";
	case EM_MICROBLAZE:
		return "Xilinx MicroBlaze";
	case EM_TILEGX:
		return "Tilera TILE-Gx";
	default:
		return "<unknown>";
	}
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
		exit(98);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1)
	{
		perror("Error: Can't open file");
		exit(98);
	}

	struct stat st;
	if (fstat(fd, &st) == -1)
	{
		perror("Error: Can't get file stats");
		close(fd);
		exit(98);
	}

	void *file_data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (file_data == MAP_FAILED)
	{
		perror("Error: mmap failed");
		close(fd);
		exit(98);
	}

	/* Check for ELF magic number. *before* casting. */
	unsigned char *magic = (unsigned char *)file_data;
	if (magic[EI_MAG0] != ELFMAG0 || magic[EI_MAG1] != ELFMAG1 ||
		magic[EI_MAG2] != ELFMAG2 || magic[EI_MAG3] != ELFMAG3)
	{
		fprintf(stderr, "Error: Not an ELF file - it has the wrong magic bytes at the start\n");
		munmap(file_data, st.st_size);
		close(fd);
		exit(98);
	}

	int is_64bit = (magic[EI_CLASS] == ELFCLASS64);
	char osabi_buffer[32];

	printf("ELF Header:\n");
	printf("  Magic:   %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
		   magic[0], magic[1], magic[2], magic[3], magic[4], magic[5], magic[6], magic[7],
		   magic[8], magic[9], magic[10], magic[11], magic[12], magic[13], magic[14], magic[15]);

	if (is_64bit)
	{
		Elf64_Ehdr *ehdr = (Elf64_Ehdr *)file_data;
		printf("  Class:                             ELF64\n");
		printf("  Data:                              %s\n", ehdr->e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian");
		printf("  Version:                           %d (current)\n", ehdr->e_ident[EI_VERSION]);
		printf("  OS/ABI:                            %s\n", get_osabi_string(ehdr->e_ident[EI_OSABI], osabi_buffer, sizeof(osabi_buffer)));
		printf("  ABI Version:                       %d\n", ehdr->e_ident[EI_ABIVERSION]);
		printf("  Type:                              %s\n", get_elf_type_string(ehdr->e_type));
		printf("  Machine:                           %s\n", get_machine_string(ehdr->e_machine));
		printf("  Version:                           0x%x\n", ehdr->e_version);
		printf("  Entry point address:               0x%lx\n", ehdr->e_entry);
		printf("  Start of program headers:          %ld (bytes into file)\n", ehdr->e_phoff);
		printf("  Start of section headers:          %ld (bytes into file)\n", ehdr->e_shoff);
		printf("  Flags:                             0x%x\n", ehdr->e_flags);
		printf("  Size of this header:               %d (bytes)\n", ehdr->e_ehsize);
		printf("  Size of program headers:           %d (bytes)\n", ehdr->e_phentsize);
		printf("  Number of program headers:         %d\n", ehdr->e_phnum);
		printf("  Size of section headers:           %d (bytes)\n", ehdr->e_shentsize);
		printf("  Number of section headers:         %d\n", ehdr->e_shnum);
		printf("  Section header string table index: %d\n", ehdr->e_shstrndx);
	}
	else
	{
		Elf32_Ehdr *ehdr = (Elf32_Ehdr *)file_data;
		printf("  Class:                             ELF32\n");
		printf("  Data:                              %s\n", ehdr->e_ident[EI_DATA] == ELFDATA2LSB ? "2's complement, little endian" : "2's complement, big endian");
		printf("  Version:                           %d (current)\n", ehdr->e_ident[EI_VERSION]);
		printf("  OS/ABI:                            %s\n", get_osabi_string(ehdr->e_ident[EI_OSABI], osabi_buffer, sizeof(osabi_buffer)));
		printf("  ABI Version:                       %d\n", ehdr->e_ident[EI_ABIVERSION]);
		printf("  Type:                              %s\n", get_elf_type_string(ehdr->e_type));
		printf("  Machine:                           %s\n", get_machine_string(ehdr->e_machine));
		printf("  Version:                           0x%x\n", ehdr->e_version);
		printf("  Entry point address:               0x%x\n", ehdr->e_entry);
		printf("  Start of program headers:          %d (bytes into file)\n", ehdr->e_phoff);
		printf("  Start of section headers:          %d (bytes into file)\n", ehdr->e_shoff);
		printf("  Flags:                             0x%x\n", ehdr->e_flags);
		printf("  Size of this header:               %d (bytes)\n", ehdr->e_ehsize);
		printf("  Size of program headers:           %d (bytes)\n", ehdr->e_phentsize);
		printf("  Number of program headers:         %d\n", ehdr->e_phnum);
		printf("  Size of section headers:           %d (bytes)\n", ehdr->e_shentsize);
		printf("  Number of section headers:         %d\n", ehdr->e_shnum);
		printf("  Section header string table index: %d\n", ehdr->e_shstrndx);
	}

	munmap(file_data, st.st_size);
	close(fd);
	return 0;
}