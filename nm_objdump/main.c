#include "hnm.h"

void print_error(const char *prog_name, const char *filename, const char *message)
{
	fprintf(stderr, "%s: ", prog_name);
	if (filename)
	{
		fprintf(stderr, "'%s': ", filename);
	}
	fprintf(stderr, "%s\n", message);
}

int process_file(const char *filename, const char *prog_name)
{
	int fd;
	unsigned char e_ident[EI_NIDENT];
	int status = EXIT_SUCCESS;
	bool is_64bit = false;
	bool swap_endian = false;
	ssize_t bytes_read;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
	{
		print_error(prog_name, filename, "No such file");
		return (EXIT_FAILURE);
	}

	bytes_read = read(fd, e_ident, EI_NIDENT);
	if (bytes_read == -1)
	{
		perror("read");
		close(fd);
		return (EXIT_FAILURE);
	}
	if (bytes_read != EI_NIDENT || memcmp(e_ident, ELFMAG, SELFMAG) != 0)
	{
		print_error(prog_name, filename, "Not an ELF file");
		close(fd);
		return (EXIT_FAILURE);
	}

	/* Determine ELF class (32/64 bit) */
	if (e_ident[EI_CLASS] == ELFCLASS64)
	{
		is_64bit = true;
	}
	else if (e_ident[EI_CLASS] != ELFCLASS32)
	{
		print_error(prog_name, filename, "Invalid ELF class");
		close(fd);
		return (EXIT_FAILURE);
	}
	/* Determine data encoding (endianness) */
	if (e_ident[EI_DATA] == ELFDATA2MSB)
	{
		swap_endian = true; /* Assuming host is little-endian */
		/* TODO: Add check for host endianness */
	}
	else if (e_ident[EI_DATA] != ELFDATA2LSB)
	{
		print_error(prog_name, filename, "Invalid data encoding");
		close(fd);
		return (EXIT_FAILURE);
	}

	/* Reset file for subsequent reads */
	if (lseek(fd, 0, SEEK_SET) == -1)
	{
		perror("lseek");
		close(fd);
		return (EXIT_FAILURE);
	}

	/* Dispatch to the right processor */
	if (is_64bit)
	{
		status = process_elf64(fd, filename, prog_name, swap_endian);
	}
	else
	{
		status = process_elf32(fd, filename, prog_name, swap_endian);
	}

	if (close(fd) == -1)
	{
		perror("close");
		if (status == EXIT_SUCCESS)
			status = EXIT_FAILURE;
	}

	return (status);
}

int main(int argc, char *argv)
{
	int i;
	int overall_status = EXIT_SUCCESS;
	const char *default_filename = "a.out";

	if (argc < 2)
	{
		/* Default behavior: process a.out if no args */
		if (process_file(default_filename, argv[0]) != EXIT_SUCCESS)
		{
			overall_status = EXIT_FAILURE;
		}
	}
	else
	{
		for (i = 1; i < argc; i++)
		{
			if (process_file(argv[i], argv[0]) != EXIT_SUCCESS)
			{
				overall_status = EXIT_FAILURE;
				/* Continue processing other files even if one fails */
			}
		}
	}

	return (overall_status);
}
