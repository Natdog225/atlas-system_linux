#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "hls.h"
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, const char *argv[])
{

	if (argc == 1 || (argc == 2 && option_one))
	{
		print_directory_contents(".", option_one);
	}
	else
	{
		int dir_count = 0;
		for (int i = 1; i < argc; i++)
		{
			if (argv[i][0] != '-')
			{
				dir_count++;
			}
		}
		{
			if (dir_count == 1)
			{
				print_directory_contents(argv[1], option_one);
			}
			else
			{
				for (int i = 1; i < argc; i++)
				{
					if (argv[i][0] == '-' && argv[i][1] == '1' && argv[i][2] == '\0')
					{
						continue;
					}

					if (lstat(argv[i], &sb) == 0)
					{
						if (S_ISDIR(sb.st_mode))
						{
							printf("%s:\n", argv[i]);
							print_directory_contents(argv[i], option_one);
							if (dir_count > 1 && i < argc - 1)
							{
								printf("\n");
							}
						}
						else
						{
							print_file_info(argv[i]);
						}
					}
					else
					{
						print_err(argv[0], argv[i]);
					}
				}
			}
		}
	}
}
}

int open_directory(const char *directory, DIR **dir)
{
	*dir = opendir(directory);
	if (*dir == NULL)
	{
		perror("./hls_01: cannot open directory");
		return -1;
	}
	return 0;
}
