#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "hls.h"

void print_directory_contents(const char *directory, int option_one)
{
	DIR *dir;

	if (open_directory(directory, &dir) == 0)
	{
		read_directory_entries(dir, option_one);
		closedir(dir);
	}
	else
	{
		print_err("./hls_02", directory);
	}
}

int open_directory(const char *directory, DIR **dir)
{
	*dir = opendir(directory);
	if (*dir == NULL)
	{
		perror("./hls_01: cannot open directory");
		return -1;
	}
	return 0;
}

void read_directory_entries(DIR *dir, int option_one)
{
	struct dirent *entry;

	while ((entry = readdir(dir)) != NULL)
	{
		/* Skip "." and ".." entries */
		if ((entry->d_name[0] == '.' && entry->d_name[1] == '\0') ||
			(entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == '\0'))
		{
			continue;
		}

		/* Skip hidden files when -1 is used */
		if (option_one && entry->d_name[0] == '.')
		{
			continue;
		}

		if (option_one)
		{
			printf("%s\n", entry->d_name); /* Print one entry per line */
		}
		else
		{
			printf("%s ", entry->d_name); /* Print space-separated */
		}
	}
	if (!option_one)
	{
		printf("\n"); /* Add newline if not -1 option */
	}
}
