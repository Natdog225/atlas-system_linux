#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "hls.h"

void print_err(const char *program, const char *path)
{
	fprintf(stderr, "%s: cannot access %s: ", program, path);
	perror(NULL);
}

int main(int argc, const char *argv[])
{
	struct stat sb;

	if (argc == 1)
	{
		print_directory_contents("."); /* List current directory */
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			if (lstat(argv[i], &sb) == 0)
			{
				if (S_ISDIR(sb.st_mode))
				{
					if (argc > 2)
					{
						printf("%s:\n", argv[i]);
					}
					print_directory_contents(argv[i]);
					if (argc > 2 && i < argc - 1)
					{
						printf("\n");
					}
				}
				else
				{
					print_file_info(argv[i]);
				}
			}
			else
			{
				print_err(argv[0], argv[i]);
			}
		}
	}
	return (0);
}
