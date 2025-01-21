#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * print_directory_contents - Prints the contents of a directory.
 * @dir_name: Name of the directory.
 * @show_hidden: Flag to show hidden files.
 * @one_per_line: Flag to print one entry per line.
 */
void print_directory_contents(char *dir_name, int show_hidden, int one_per_line)
{
	DIR *dir;
	struct dirent *entry;

	dir = opendir(dir_name);
	if (dir == NULL)
	{
		fprintf(stderr, "%s: cannot access '%s': %s\n",
			"./hls_01", dir_name, strerror(errno));
		exit(1);
	}

	while ((entry = readdir(dir)) != NULL)
	{
		if (show_hidden == 0 && entry->d_name[0] == '.')
		{
			continue;
		}

		printf("%s", entry->d_name);

		if (one_per_line)
		{
			printf("\n");
		}
		else
		{
			printf("  ");
		}
	}

	if (!one_per_line)
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
	int one_per_line = 0;
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
			else if (argv[i][0] == '-' && argv[i][1] == '1' && argv[i][2] == '\0')
			{
				one_per_line = 1;
			}
		}
		else
		{
			if (lstat(argv[i], &path_stat) == -1)
			{
				fprintf(stderr, "%s: cannot access '%s': %s\n",
					"./hls_01", argv[i], strerror(errno));
				exit(1);
			}

			if (S_ISDIR(path_stat.st_mode))
			{
				if (argc > 2)
				{
					printf("%s:\n", argv[i]);
				}
				print_directory_contents(argv[i], show_hidden, one_per_line);
			}
			else
			{
				printf("%s\n", argv[i]);
			}
		}
	}

	return (0);
}
