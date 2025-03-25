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
