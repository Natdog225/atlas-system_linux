#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * print_directory_contents - Prints the contents of a directory
 * @dir_name: Name of the directory
 * @show_hidden: Flag to show hidden files
 */
void print_directory_contents(char *dir_name, int show_hidden)
{
	DIR *dir;
	struct dirent *entry;
	int count = 0;

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
		if (count > 0)
		{
			printf("  ");
		}
		printf("%s", entry->d_name);
		count++;
	}
	if (count > 0)
	{
		printf("\n");
	}
	closedir(dir);
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
	struct stat path_stat;
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][0] == '-' && argv[i][1] == 'a' && argv[i][2] == '\0')
			{
				show_hidden = 1;
			}
		}
		else
		{
			if (argc > 2) /* Only print colon if more than one path could be fix? */
			{
				printf("%s:\n", argv[i]);
			}

			if (lstat(argv[i], &path_stat) == -1)
			{
				fprintf(stderr, "%s: cannot access %s: %s\n",
						argv[0], argv[i], strerror(errno));
				exit(1);
			}

			if (S_ISDIR(path_stat.st_mode))
			{
				print_directory_contents(argv[i], show_hidden);
			}
			else
			{
				printf("%s\n", argv[i]);
			}
		}
	}

	return (0);
}
