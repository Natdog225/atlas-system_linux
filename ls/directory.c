#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "hls.h"

int scan_sort(const struct dirent **a, const struct dirent **b)
{
	return strcasecmp((*a)->d_name, (*b)->d_name);
}

int quick_sort(const void *a, const void *b)
{
	const struct dirent *dir_a = *(const struct dirent **)a;
	const struct dirent *dir_b = *(const struct dirent **)b;

	/* case insensitive */
	return strcasecmp(dir_a->d_name, dir_b->d_name);
}
/* Prints the contents of a directory. */
void print_directory_contents(const char *path, int option_one, int hidden, int option_A)
{
	DIR *dir;
	struct dirent *entry;
	struct dirent **sort_name;
	int n;

	if ((dir = opendir(path)) == NULL)
	{
		print_err("./hls_04", path);
		return;
	}
	n = scandir(path, &sort_name, NULL, scan_sort);
	if (n < 0)
	{
		perror("scandir");
		return;
	}

	for (int i = 0; i < n; i++)
	{
		entry = sort_name[i];

		if (!entry) {
			continue;
		}

		if ((!hidden && !option_A && option_one && *entry->d_name == '.') ||
			/* Skip "." and ".." when -A is used */
			(option_A &&
			 ((*entry->d_name == '.' && *(entry->d_name + 1) == '\0') ||
				 (*entry->d_name == '.' && *(entry->d_name + 2) == '\0' && *(entry->d_name + 3) == '\0'))))
		{
			free(entry);
			continue;
		}

		if (option_one || !hidden)
		{
			printf("%s\n", entry->d_name);
		}
		else
		{
			struct stat sb;
			const char *entry_path = path_join(path, entry->d_name);
			if (lstat(entry_path, &sb) == -1)
			{
				print_err("./hls_04", entry_path);
			}
			print_long_format(&sb, entry->d_name);
		}

		free(entry);
	}

	// qsort(sort_name, n, sizeof(struct dirent*), quick_sort);
	// for (int i = 0; i < n; i++)
	// {
	//     printf("%s\n", sort_name[i]->d_name);
	// }

	free(sort_name);
	closedir(dir);
}

/* Opens a directory. */
int open_directory(const char *directory, DIR **dir)
{
	*dir = opendir(directory);
	if (*dir == NULL)
	{
		perror(directory);
		return -1;
	}
	return 0;
}

/* Reads and prints directory entries. */
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
		if (option_one && entry->d_name[0] == '.' && entry->d_name[1] != '\0')
		{
			continue;
		}

		printf("%s ", entry->d_name); /* Print space-separated */
	}
	if (!option_one)
	{
		printf("\n"); /* Add newline if not -1 option */
	}
}
