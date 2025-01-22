#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_FILES 1000

/**
 * custom_strcmp - Custom strcmp implementation
 * @s1: First string
 * @s2: Second string
 *
 * Return: Result of comparison
 */
int custom_strcmp(const char *s1, const char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

/**
 * compare_strings - Helper function for qsort to compare strings
 * @a: First string
 * @b: Second string
 *
 * Return: Result of custom_strcmp
 */
int compare_strings(const void *a, const void *b)
{
	return (custom_strcmp(*(char **)a, *(char **)b));
}

/**
 * print_directory_contents - Prints the contents of a directory
 * @dir_name: Name of the directory
 * @show_hidden: Flag to show hidden files
 * @one_per_line: Flag to print one entry per line
 */
void print_directory_contents(char *dir_name, int show_hidden, int one_per_line)
{
	DIR *dir;
	struct dirent *entry;
	char filenames[MAX_FILES][1024];
	int count = 0;
	int i;

	dir = opendir(dir_name);
	if (dir == NULL)
	{
		perror("opendir");
		exit(1);
	}

	while ((entry = readdir(dir)) != NULL)
	{
		if (show_hidden == 0 && entry->d_name[0] == '.')
		{
			continue;
		}

		strcpy(filenames[count], entry->d_name);
		count++;
		if (count >= MAX_FILES)
		{
			break;
		}
	}

	closedir(dir);

	qsort(filenames, count, sizeof(filenames[0]), compare_strings);

	for (i = 0; i < count; i++)
	{
		printf("%s%s", filenames[i], one_per_line ? "\n" : "  ");
	}

	if (!one_per_line)
	{
		printf("\n");
	}
}

/**
 * main - Entry point
 * @argc: Argument count
 * @argv: Argument vector
 *
 * Return: Always 0
 */
int main(int argc, char *argv[])
{
	int show_hidden = 0;
	int one_per_line = 0;
	struct stat path_stat;
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (custom_strcmp(argv[i], "-a") == 0)
			{
				show_hidden = 1;
			}
			else if (custom_strcmp(argv[i], "-1") == 0)
			{
				one_per_line = 1;
			}
			else
			{
				fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], argv[i][1]);
				exit(1);
			}
		}
		else
		{
			break; /* Stop parsing options when a non-option is encountered */
		}
	}

	for (; i < argc; i++)
	{
		if (lstat(argv[i], &path_stat) == -1)
		{
			fprintf(stderr, "%s: cannot access %s: %s\n",
				argv[0], argv[i], strerror(errno));
			continue; /* Continue to the next argument if lstat fails */
		}

		if (S_ISDIR(path_stat.st_mode))
		{
			printf("%s:\n", argv[i]);
			print_directory_contents(argv[i], show_hidden, one_per_line);
		}
		else
		{
			printf("%s\n", argv[i]);
		}
	}

	return (0);
}
