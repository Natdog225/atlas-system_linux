#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "hls.h"

static char PATH_BUF[PATH_MAX];
static char PERMS_ALPHAMAP[] = {'r', 'w', 'x'};
static char FTYPE_ALPHAMAP[] = {
	'?', 'p', 'c', '?', 'd', '?', 'b', '?', '-', '?', 'l', '?', 's', '?', '?',
	'?', 'p', 'c', '?', 'd', '?', 'b', '?', '-', '?', 'l', '?', 's', '?', '?',
	'?', 'p', 'c', '?', 'd', '?', 'b', '?', '-', '?', 'l', '?', 's', '?', '?',
	'?', 'p', 'c', '?', 'd', '?', 'b', '?', '-', '?', 'l', '?', 's', '?', '?'};

/*
 * Returns a string representation of the file type.
 */
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

/*
 * Concatenates two path components.
 */
const char *path_join(const char *dirpath, const char *entry_name)
{
	char *dest = PATH_BUF;

	while (*dirpath)
	{
		*dest++ = *dirpath++;
	}

	*dest++ = '/';

	while (*entry_name)
	{
		*dest++ = *entry_name++;
	}

	*dest = '\0';

	return PATH_BUF;
}

/*
 * Converts file mode to a string representation.
 */
int mode_to_str(char *buf, mode_t mode)
{
	mode_t pmask, i;

	*buf++ = FTYPE_ALPHAMAP[(mode & S_IFMT) >> 12];

	pmask = S_IRUSR;

	for (i = 0; i < 9; ++i, pmask >>= 1)
		*buf++ = (mode & pmask) ? PERMS_ALPHAMAP[i % 3] : '-';

	*buf = '\0';

	return (0);
}

/*
 * Initializes a longlistfmt_t struct.
 */
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

/*
 * Prints the contents of a longlistfmt_t struct.
 */
void longlistfmt_print(longlistfmt_t *longlist)
{
	printf("%s %lu %s %s %-4ld %.12s %s\n", longlist->mode, longlist->nlinks,
		   longlist->user, longlist->group, longlist->size,
		   longlist->modified + 4, longlist->entry_name);
}

/*
 * Prints file information in long format.
 */
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

/*
 * Prints file information.
 */
void print_file_info(const char *path)
{
	struct stat sb;
	if (lstat(path, &sb) == -1)
	{
		print_err("./hls_04", path);
		return;
	}

	print_long_format(&sb, path);
}

/*
 * Prints an error message.
 */
void print_err(const char *program, const char *path)
{
	fprintf(stderr, "%s: cannot access %s: ", program, path);
	perror(NULL);
}

/*
 * Main function.
 */
int main(int argc, char *argv)
{
	struct stat sb;
	int option_one = 0;
	int hidden = 0;
	int option_A = 0;
	int option_l = 0;
	int dir_count = 0;
	int file_count = 0;
	char **files = malloc(argc * sizeof(char *));
	char **dirs = malloc(argc * sizeof(char *));

	if (files == NULL || dirs == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}

	/* Parse command line options */
	for (int i = 1; i < argc; i++)
	{
		/* Check for -1 option */
		if (argv[i] == '-' && argv[i] == '1' && argv[i] == '\0')
		{
			option_one = 1;
		}
		/* Check for -a option */
		else if (argv[i] == '-' && argv[i] == 'a' && argv[i] == '\0')
		{
			hidden = 1;
		}
		/* Check for -A option */
		else if (argv[i] == '-' && argv[i] == 'A' && argv[i] == '\0')
		{
			option_A = 1;
		}
		/* Check for -l option */
		else if (argv[i] == '-' && argv[i] == 'l' && argv[i] == '\0')
		{
			option_l = 1;
		}
		else
		{
			/* Handle invalid options */
			if (argv[i] == '-' && ((argv[i] != '1' && argv[i] != 'a' && argv[i] != 'A' && argv[i] != 'l') || argv[i] != '\0'))
			{
				print_err(argv, argv[i]); /* Print an error for invalid options */
			}
			else if (argv[i] != '-')
			{
				if (lstat(argv[i], &sb) == 0)
				{
					if (S_ISDIR(sb.st_mode))
					{
						dirs[dir_count++] = argv[i];
					}
					else
					{
						files[file_count++] = argv[i];
					}
				}
				else
				{
					print_err(argv, argv[i]); /* Print an error for invalid paths */
				}
			}
		}
	}

	/* Print files first */
	for (int i = 0; i < file_count; i++)
	{
		print_file_info(files[i]);
	}

	/* Print directories */
	for (int i = 0; i < dir_count; i++)
	{
		if (dir_count > 1)
		{
			printf("%s:\n", dirs[i]);
		}
		print_directory_contents(dirs[i], option_one, hidden, option_A, option_l);
		/* Print newline between directories */
		if (dir_count > 1 && i < dir_count - 1)
		{
			printf("\n");
		}
	}

	free(files);
	free(dirs);
	return (0);
}
