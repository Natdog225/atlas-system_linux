#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_FILES 1000

int compare_strings(const void *a, const void *b)
{
	return strcmp(*(char **)a, *(char **)b);
}

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
			continue;

		strcpy(filenames[count], entry->d_name);
		count++;
		if (count >= MAX_FILES)
			break;
	}

	closedir(dir);

	qsort(filenames, count, sizeof(filenames[0]), compare_strings);

	for (i = 0; i < count; i++)
		printf("%-*s%s", 20, filenames[i], one_per_line ? "\n" : "  ");

	printf("\n");
}

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
			if (strcmp(argv[i], "-a") == 0)
				show_hidden = 1;
			else if (strcmp(argv[i], "-1") == 0)
				one_per_line = 1;
			else
			{
				fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], argv[i][1]);
				exit(1);
			}
		}
		else
			break;
	}


	for (; i < argc; i++)
	{
		if (lstat(argv[i], &path_stat) == -1)
		{
			fprintf(stderr, "%s: cannot access %s: %s\n",
				argv[0], argv[i], strerror(errno));
			continue;
		}

		if (S_ISDIR(path_stat.st_mode))
		{
			printf("%s:\n", argv[i]);
			print_directory_contents(argv[i], show_hidden, one_per_line);
		}
		else
			printf("%s\n", argv[i]);
	}

	return (0);
}
