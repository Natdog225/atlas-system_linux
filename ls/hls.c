#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
/**
 * main - Entry point
 *
 * Return: Always 0
 */
int main(void)
{
	DIR *dir;
	struct dirent *entry;

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
		printf("%s  ", entry->d_name);
	}
	printf("\n");

	/* Close the directory */
	closedir(dir);

	return (0);
}
