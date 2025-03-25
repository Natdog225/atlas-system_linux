#include "hnm.h"

void swap_endian_elf32_ehdr(Elf32_Ehdr *ehdr)
{
	ehdr->e_type      = __builtin_bswap16(ehdr->e_type);
	ehdr->e_machine   = __builtin_bswap16(ehdr->e_machine);
	ehdr->e_version   = __builtin_bswap32(ehdr->e_version);
	ehdr->e_entry     = __builtin_bswap32(ehdr->e_entry);
	ehdr->e_phoff     = __builtin_bswap32(ehdr->e_phoff);
	ehdr->e_shoff     = __builtin_bswap32(ehdr->e_shoff);
	ehdr->e_flags     = __builtin_bswap32(ehdr->e_flags);
	ehdr->e_ehsize    = __builtin_bswap16(ehdr->e_ehsize);
	ehdr->e_phentsize = __builtin_bswap16(ehdr->e_phentsize);
	ehdr->e_phnum     = __builtin_bswap16(ehdr->e_phnum);
	ehdr->e_shentsize = __builtin_bswap16(ehdr->e_shentsize);
	ehdr->e_shnum     = __builtin_bswap16(ehdr->e_shnum);
	ehdr->e_shstrndx  = __builtin_bswap16(ehdr->e_shstrndx);
}
void swap_endian_elf32_shdr(Elf32_Shdr *shdr)
{
	shdr->sh_name      = __builtin_bswap32(shdr->sh_name);
	shdr->sh_type      = __builtin_bswap32(shdr->sh_type);
	shdr->sh_flags     = __builtin_bswap32(shdr->sh_flags);
	shdr->sh_addr      = __builtin_bswap32(shdr->sh_addr);
	shdr->sh_offset    = __builtin_bswap32(shdr->sh_offset);
	shdr->sh_size      = __builtin_bswap32(shdr->sh_size);
	shdr->sh_link      = __builtin_bswap32(shdr->sh_link);
	shdr->sh_info      = __builtin_bswap32(shdr->sh_info);
	shdr->sh_addralign = __builtin_bswap32(shdr->sh_addralign);
	shdr->sh_entsize   = __builtin_bswap32(shdr->sh_entsize);
}
void swap_endian_elf32_sym(Elf32_Sym *sym)
{
	sym->st_name  = __builtin_bswap32(sym->st_name);
	sym->st_value = __builtin_bswap32(sym->st_value);
	sym->st_size  = __builtin_bswap32(sym->st_size);
	sym->st_shndx = __builtin_bswap16(sym->st_shndx);
}
