#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>

/**
 * print_long_format - Prints file information in long format
 * @sb: Pointer to stat structure
 * @name: Name of the file
 */
void print_long_format(struct stat *sb, const char *name)
{
	static char perms[] = "----------";
	static const char *const fmt[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
	struct passwd *pw;
	struct group *gr;
	char date[32];

	strftime(date, sizeof(date), "%b %d %H:%M", localtime(&sb->st_mtime));

	perms[0] = (sb->st_mode & S_IFDIR) ? 'd' : '-';
	perms[1] = fmt[(sb->st_mode >> 6) & 07][0];
	perms[2] = fmt[(sb->st_mode >> 6) & 07][1];
	perms[3] = fmt[(sb->st_mode >> 6) & 07][2];
	perms[4] = fmt[(sb->st_mode >> 3) & 07][0];
	perms[5] = fmt[(sb->st_mode >> 3) & 07][1];
	perms[6] = fmt[(sb->st_mode >> 3) & 07][2];
	perms[7] = fmt[sb->st_mode & 07][0];
	perms[8] = fmt[sb->st_mode & 07][1];
	perms[9] = fmt[sb->st_mode & 07][2];

	pw = getpwuid(sb->st_uid);
	gr = getgrgid(sb->st_gid);

	printf("%s %lu %s %s %zu %s %s\n",
		perms, sb->st_nlink,
		pw ? pw->pw_name : "",
		gr ? gr->gr_name : "",
		sb->st_size, date, name);
}

/**
 * print_file_info - Prints file information
 * @path: Path to the file
 * @name: Name of the file
 * @show_hidden: Flag to show hidden files
 */
void print_file_info(const char *path, const char *name, int show_hidden)
{
	DIR *dir;
	struct dirent *entry;
	struct stat sb;
	char full_path[PATH_MAX];

	if (lstat(path, &sb) == -1)
	{
		fprintf(stderr, "./hls: cannot access '%s': %s\n", path, strerror(errno));
		exit(1);
	}

	if (S_ISDIR(sb.st_mode))
	{
		dir = opendir(path);
		if (dir == NULL)
		{
			perror("opendir");
			exit(1);
		}
		while ((entry = readdir(dir)) != NULL)
		{
			if (!show_hidden && entry->d_name[0] == '.')
			{
				continue;
			}
			snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

			if (lstat(full_path, &sb) == -1)
			{
				fprintf(stderr, "./hls: cannot access '%s': %s\n",
					full_path, strerror(errno));
				exit(1);
			}
			print_long_format(&sb, entry->d_name);
		}
		closedir(dir);
	}
	else
	{
		print_long_format(&sb, name);
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
	int show_all = 0;
	/*int long_format = 0; */
	int opt;
	int i;

	while ((opt = getopt(argc, argv, "al")) != -1)
	{
		switch (opt)
		{
		case 'a':
			show_all = 1;
			break;
		case 'l':
			long_format = 1;
			break;
		default:
			fprintf(stderr, "Usage: %s [-al] [FILE...]\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if (optind >= argc)
	{
		print_file_info(".", ".", show_all);
	}
	else
	{
		for (i = optind; i < argc; i++)
		{
			print_file_info(argv[i], argv[i], show_all);
		}
	}

	return (0);
}
