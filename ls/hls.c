#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

static char PATH_BUF[PATH_MAX];
static char PERMS_ALPHAMAP[3] = {'r', 'w', 'x'};
static char FTYPE_ALPHAMAP[16] = {
	'?', 'p', 'c', '?', 'd', '?', 'b', '?', '-', '?', 'l', '?', 's', '?', '?',
	'?'};

const char *dirent_type_name(unsigned char d_type)
{
	switch (d_type)
	{
	case DT_BLK:
		return "block_device";
	case DT_CHR:
		return "character_device";
	case DT_DIR:
		return "directory";
	case DT_FIFO:
		return "named_pipe";
	case DT_LNK:
		return "symbolic_link";
	case DT_REG:
		return "file";
	case DT_SOCK:
		return "socket";
	default: /* DT_UNKNOWN */
		return "unknown";
	}
}

	const char *path_join(const char *dirpath, const char *entry_name)
	{
		char *dest = PATH_BUF;

		/* Copy dirpath to PATH_BUF */
		while (*dirpath)
		{
			*dest++ = *dirpath++;
		}

		/* Add the '/' separator */
		*dest++ = '/';

		/* Copy entry_name to PATH_BUF */
		while (*entry_name)
		{
			*dest++ = *entry_name++;
		}

		/* Null-terminate the string */
		*dest = '\0';

		return PATH_BUF;
	}
}

int mode_to_str(char *buf, mode_t mode)
{
	mode_t pmask, i;

	/* maps the file-type bits to a value 0-15 */
	*buf++ = FTYPE_ALPHAMAP[(mode & S_IFMT) >> 12];

	/* masks read permissions of owner first */
	pmask = S_IRUSR;

	/**
	 * check each permission bit and map to alpha code
	 * owner:  i = 0-2 (rwx)
	 * group:  i = 3-5 (rwx)
	 * others: i = 6-8 (rwx)
	 */
	for (i = 0; i < 9; ++i, pmask >>= 1)
		*buf++ = (mode & pmask) ? PERMS_ALPHAMAP[i % 3] : '-';

	*buf = '\0';

	return (0);
}

typedef struct longlistfmt
{
	char mode[16];
	nlink_t nlinks;
	const char *user;
	const char *group;
	off_t size;
	const char *modified;
	const char *entry_name;
} longlistfmt_t;

int longlistfmt_init(longlistfmt_t *longlist, const char *entry_name,
					 struct stat *statbuf)
{
	struct passwd *pwd = NULL;
	struct group *group = NULL;

	mode_to_str(longlist->mode, statbuf->st_mode);
	longlist->nlinks = statbuf->st_nlink;

	pwd = getpwuid(statbuf->st_uid);
	group = getgrgid(statbuf->st_gid);

	if (!pwd || !group)
		return (-1);

	longlist->user = pwd->pw_name;
	longlist->group = group->gr_name;
	longlist->size = statbuf->st_size;
	longlist->modified = ctime(&(statbuf->st_mtime));
	longlist->entry_name = entry_name;
	return (0);
}

void longlistfmt_print(longlistfmt_t *longlist)
{
	/* mode nlink user group size modified entry_name */
	printf("%s %lu %s %s %-4lu %.12s %s\n", longlist->mode, longlist->nlinks,
		   longlist->user, longlist->group, longlist->size,
		   /* trimming the \n off of ctime string */
		   longlist->modified + 4, longlist->entry_name);
}

void print_long_format(struct stat *sb, const char *name)
{
	longlistfmt_t longlist;

	if (longlistfmt_init(&longlist, name, sb) == 0)
	{
		longlistfmt_print(&longlist);
	}
	else
	{
		perror("longlistfmt_init");
	}
}

void print_file_info(const char *path, const char *name, int show_hidden)
{
	DIR *dir;
	struct dirent *entry;
	struct stat sb;

	if (lstat(path, &sb) == -1)
	{
		fprintf(stderr, "./hls: cannot access '%s': Error %d\n", path, errno);
		exit(1);
	}

	if (S_ISDIR(sb.st_mode))
	{
		dir = opendir(path);
		if (dir == NULL)
		{
			fprintf(stderr, "./hls: cannot access '%s': Error %d\n", path, errno);
			exit(1);
		}

		while ((entry = readdir(dir)) != NULL)
		{
			if (!show_hidden && entry->d_name[0] == '.')
			{
				continue;
			}

			const char *full_path = path_join(path, entry->d_name);

			if (lstat(full_path, &sb) == -1)
			{
				fprintf(stderr, "./hls: cannot access '%s': Error %d\n", full_path,
						errno);
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
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'a' && argv[i][2] == '\0')
			{
				show_all = 1;
			}
			else
			{
				fprintf(stderr, "Usage: %s [-al] [FILE...]\n", argv[0]);
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			break; /* Stop parsing options when a non-option is encountered */
		}
	}

	if (i == 1) /* No paths provided, list current directory */
	{
		print_file_info(".", ".", show_all);
	}
	else
	{
		for (; i < argc; i++)
		{
			print_file_info(argv[i], argv[i], show_all);
		}
	}

	return (0);
}
