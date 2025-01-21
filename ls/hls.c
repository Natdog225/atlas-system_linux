#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define MAX_FILES 1000
void print_directory_contents(char *dir_name, int show_hidden)
{
    DIR *dir;
    struct dirent *entry;
    char buffer[1024];
    char filenames[MAX_FILES][1024];

    dir = opendir(dir_name);
    if (dir == NULL)
    {
        perror("opendir");
        exit(1);
    }

    int count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (show_hidden == 0 && entry->d_name[0] == '.')
        {
            continue;
        }

		// Convert filename to lowercase
		for (int i = 0; i < strlen(entry->d_name); i++)
		{
			buffer[i] = tolower(entry->d_name[i]);
		}
		buffer[strlen(entry->d_name)] = '\0';

		// Store filename
		strcpy(filenames[count], buffer);
		count++;

		if (count >= MAX_FILES)
		{
			break;
		}
	}

	closedir(dir);

	// Sort filenames alphabetically
	qsort(filenames, count, sizeof(char*), compare_strings);

	// Print sorted filenames
	for (int i = 0; i < count; i++)
	{
		snprintf(buffer, sizeof(buffer), "%-*s\n", 20, filenames[i]);
		fwrite(buffer, 1, strlen(buffer), stdout);
	}
	closedir(dir);
}

// Helper function for qsort
int compare_strings(const void *a, const void *b)
{
    return strcmp(*(char**)a, *(char**)b);
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
			if (argc > 2) /* Only print colon if more than one path, could be fix? */
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
