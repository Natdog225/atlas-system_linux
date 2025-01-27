#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "hls.h"

void print_directory_contents(const char *directory, int option_one)
{
    DIR *dir;
	struct dirent *entry;


	if (open_directory(directory, &dir) == 0)
	{
		while ((entry = readdir(dir)) != NULL)
		{
			/* Skip "." and ".." entries */
			if (entry->d_name[0] == '.' && entry->d_name[1] == '\0' ||
				entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == '\0')
			{
				continue;
			}

			/* Skip hidden files when -1 is used */
			if (option_one && entry->d_name[0] == '.')
			{
				continue;
			}

			printf("%s ", entry->d_name); /* Print space-separated */
		}
		printf("\n"); /* Add newline if not -1 option */
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
