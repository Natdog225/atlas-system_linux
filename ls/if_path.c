#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include "hls.h"

void if_path(const char *path, const char *program, int hidden)
{
	struct stat sb;

	if (lstat(path, &sb) == 0)
	{
		if (S_ISDIR(sb.st_mode))
		{
			/* this prints dir name */
			/* printf("%s\n", path); */
			print_directory_contents(path, 0, hidden, 0);
		}
		else if (S_ISREG(sb.st_mode))
		{
			if (hidden || path[0] != '.')
			{
				printf("%s\n", path);
			}
		}
		else
		{
			print_err(program, path);
		}
	}
	else
	{
		print_err(program, path);
	}
}
