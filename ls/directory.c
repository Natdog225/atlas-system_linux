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
{ /* Add option_one parameter */
	struct dirent *entry;

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}
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
