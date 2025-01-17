#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
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
	DIR *dir;
	struct dirent *entry;
	char *path = ".";
	struct stat path_stat;

	/*Parse command-line arguments*/
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] != '-')
		{
\			path = argv[i];
		}
		else if (argv[i][0] == '-' && argv[i][1] == 'a' && argv[i][2] == '\0')
		{
			show_hidden = 1;
		}
	}
	if (lstat(path, &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
	{
		dir = opendir(path);
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
			printf("%s  ", entry->d_name);
		}
		printf("\n");

		closedir(dir);
		return (0);
	}
