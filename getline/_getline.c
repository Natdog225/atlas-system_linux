#include "_getline.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINES 25

char *_getline(const int fd)
{
	static char buffer[MAX_FDS][READ_SIZE + 1] = {{0}};
	static int read_bytes[MAX_FDS] = {0};
	static int offset[MAX_FDS] = {0};
	static int line_count = 0;
	char *line = NULL;
	int line_len = 0;

	if (fd < 0 || fd >= MAX_FDS)
	{
		fprintf(stderr, "Invalid file descriptor\n");
		return NULL;
	}

	if (line_count >= MAX_LINES)
	{
		return NULL;
	}

	while (1)
	{
		if (offset[fd] >= read_bytes[fd])
		{
			offset[fd] = 0;
			read_bytes[fd] = read(fd, buffer[fd], READ_SIZE);
			if (read_bytes[fd] <= 0)
			{
				if (line_len == 0)
				{
					return NULL;
				}
				break;
			}
			buffer[fd][read_bytes[fd]] = '\0';
		}

		for (int i = offset[fd]; i < read_bytes[fd]; i++)
		{
			line = realloc(line, line_len + 2);
			if (!line)
			{
				perror("Memory allocation failed");
				return NULL;
			}

			line[line_len++] = buffer[fd][i];

			if (buffer[fd][i] == '\n')
			{
				line[line_len - 1] = '\n';
				line[line_len] = '\0';
				offset[fd] = i + 1;
				line_count++;
				return line;
			}
		}
		offset[fd] = read_bytes[fd];
	}

	/*EOF case if no new lines*/
	if (line_len > 0)
	{
		line = realloc(line, line_len + 1);
		if (!line)
		{
			perror("Memory allocation failed");
			return NULL;
		}
		line[line_len] = '\0';
		line_count++;
		return line;
	}

	return NULL;
}
