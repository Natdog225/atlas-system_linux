#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "hls.h"

void print_directory_contents(const char *directory)
{
	DIR *dir;

	if (open_directory(directory, &dir) == 0)
	{
		read_directory_entries(dir);
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

void read_directory_entries(DIR *dir)
{
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		/* Skip "." and ".." entries */
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		{
			continue;
		}

		printf("%s ", entry->d_name);
	}
	printf("\n");
}
