#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] == 'a' && argv[i][2] == '\0')
		{
			show_hidden = 1;
		}
	}
	/* Open the current directory */
	dir = opendir(".");
	if (dir == NULL)
	{
		perror("opendir");
		exit(1);
	}

	/* Read directory entries */
	while ((entry = readdir(dir)) != NULL)
	{
		if (show_hidden == 0 && entry->d_name[0] == '.')
		{
			continue;
		}
		printf("%s  ", entry->d_name);
	}
	printf("\n");

	/* Close the directory */
	closedir(dir);

	return (0);
}
