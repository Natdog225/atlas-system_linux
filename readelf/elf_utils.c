#include "elf_utils.h"
#include <stdio.h>
#include <stdint.h>

/**
 * get_osabi_string - Returns the OS/ABI string.
 * @osabi: The OS/ABI byte.
 * @buffer: Buffer to store the string.
 * @buf_size: Size of the buffer.
 *
 * Return: Pointer to the string.
 */
const char *get_osabi_string(unsigned char osabi, char *buffer, size_t buf_size)
{
	switch (osabi)
	{
	case ELFOSABI_SYSV:
		return ("UNIX - System V");
	case ELFOSABI_HPUX:
		return ("UNIX - HP-UX");
	case ELFOSABI_NETBSD:
		return ("UNIX - NetBSD");
	case ELFOSABI_LINUX:
		return ("UNIX - Linux");
	case ELFOSABI_SOLARIS:
		return ("UNIX - Solaris");
	case ELFOSABI_IRIX:
		return ("UNIX - IRIX");
	case ELFOSABI_FREEBSD:
		return ("UNIX - FreeBSD");
	case ELFOSABI_TRU64:
		return ("UNIX - TRU64");
	case ELFOSABI_ARM:
		return ("ARM");
	case ELFOSABI_STANDALONE:
		return ("Standalone App");
	default:
		/* Use the raw byte value directly */
		snprintf(buffer, buf_size, "<unknown: %u>", osabi);
		return (buffer);
	}
}

/**
 * get_elf_type_string - Returns the ELF type string.
 * @type: The ELF type value.
 *
 * Return: Pointer to the string.
 */
const char *get_elf_type_string(unsigned short type)
{
	switch (type)
	{
	case ET_NONE:
		return ("NONE (No file type)");
	case ET_REL:
		return ("REL (Relocatable file)");
	case ET_EXEC:
		return ("EXEC (Executable file)");
	case ET_DYN:
		return ("DYN (Shared object file)");
	case ET_CORE:
		return ("CORE (Core file)");
	default:
		return ("<unknown>");
	}
}

/**
 * get_machine_string - Returns the machine architecture string.
 * @machine: The machine architecture value.
 *
 * Return: Pointer to the string.
 */
const char *get_machine_string(unsigned short machine)
{
	switch (machine)
	{
	case EM_NONE:
		return ("None");
	case EM_M32:
		return ("WE32100");
	case EM_SPARC:
		return ("Sparc");
	case EM_386:
		return ("Intel 80386");
	case EM_68K:
		return ("MC68000");
	case EM_88K:
		return ("MC88000");
	case EM_860:
		return ("Intel 80860");
	case EM_MIPS:
		return ("MIPS R3000");
	case EM_S370:
		return ("IBM System/370");
	case EM_MIPS_RS3_LE:
		return ("MIPS R4000 big-endian");
	case EM_PARISC:
		return ("HPPA");
	case EM_SPARC32PLUS:
		return ("Sparc v8+");
	case EM_960:
		return ("Intel 80960");
	case EM_PPC:
		return ("PowerPC");
	case EM_PPC64:
		return ("PowerPC64");
	case EM_FR20:
		return ("Fujitsu FR20");
	case EM_S390:
		return ("IBM S/390");
	case EM_SPU:
		return ("SPU");
	case EM_V800:
		return ("Renesas V850");
	case EM_IA_64:
		return ("Intel IA-64");
	case EM_X86_64:
		return ("Advanced Micro Devices X86-64");
	case EM_VAX:
		return ("Digital VAX");
	case EM_CRIS:
		return ("Axis Comm. CRIS Architecture");
	case EM_JAVELIN:
		return ("Infineon Tech. Javelin Processor");
	case EM_FIREPATH:
		return ("Element 14 6500");
	case EM_ZSP:
		return ("LSI Logic's 16-bit DSP Processor");
	case EM_MMIX:
		return ("Donald Knuth's educational 64-bit proc");
	case EM_HUANY:
		return ("Harvard University machine-independent object files");
	case EM_PRISM:
		return ("SiTera Prism");
	case EM_ARM:
		return ("Advanced RISC Machines ARM");
	case EM_SH:
		return ("Renesas SuperH");
	case EM_SPARCV9:
		return ("SPARC v9 64-bit");
	case EM_TRICORE:
		return ("Siemens Tricore embedded processor");
	case EM_ARC:
		return ("Argonaut RISC Core");
	case EM_H8_300:
		return ("Renesas H8/300");
	case EM_H8_300H:
		return ("Renesas H8/300H");
	case EM_H8S:
		return ("Renesas H8S");
	case EM_H8_500:
		return ("Renesas H8/500");
	case EM_AARCH64:
		return ("ARM aarch64");
	case EM_TILEPRO:
		return ("Tilera TILEPro");
	case EM_MICROBLAZE:
		return ("Xilinx MicroBlaze");
	case EM_TILEGX:
		return ("Tilera TILE-Gx");
	default:
		return ("<unknown>");
	}
}

/**
 * read16 - Reads a 16-bit value with correct endianness.
 * @data: Pointer to the data.
 * @big_endian: 1 if big-endian, 0 if little-endian.
 *
 * Return: The 16-bit value.
 */
uint16_t read16(const void *data, int big_endian)
{
	const uint8_t *bytes = (const uint8_t *)data;

	return (big_endian ? (bytes[0] << 8) | bytes[1] : /* Big-endian */
				(bytes[1] << 8) | bytes[0]);		  /* Little-endian */
}

/**
 * read32 - Reads a 32-bit value with correct endianness.
 * @data: Pointer to the data.
 * @big_endian: 1 if big-endian, 0 if little-endian.
 *
 * Return: The 32-bit value.
 */
uint32_t read32(const void *data, int big_endian)
{
	const uint8_t *bytes = (const uint8_t *)data;

	return (big_endian ? (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3] : /* Big-endian */
				(bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0]);			/* Little-endian*/
}