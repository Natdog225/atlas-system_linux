#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "hls.h"

void print_err(const char *program, const char *path, const char *error_mess)
{
	fprintf(stderr, "%s: cannot access %s: %s\n", program, path, error_mess);
}

int main(int argc, const char *argv[])
{
	struct stat sb;

	if (argc == 1)
	{
		print_directory_contents("");
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (lstat(argv[i], &sb) == 0)
			{
				/* Print the directory name only if there are multiple arguments */
				/* or if it is a directory */
				/* added but still wrong. need seperate conditions */
				/* print_directory_contents(argv[i]); */
				if (argc > 2)
				{ /* ||  (argc == 2 && S_ISDIR(sb.st_mode))) { */
					printf("%s:\n", argv[i]);
				}
				if_path(argv[i], argv[0]);
				if (argc > 2 && i < argc - 1)
				{
					printf("\n");
				}
			}
			else
			{
				print_err(argv[0], argv[i], strerror(errno));
			}
		}
	}
	return (0);
}