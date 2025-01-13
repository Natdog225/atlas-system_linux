#include "_getline.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 25
#define READ_SIZE 1024
#define MAX_FDS 256

static char buffer[MAX_FDS][READ_SIZE + 1] = {{0}};
static int read_bytes[MAX_FDS] = {0};
static int offset[MAX_FDS] = {0};
static int total_lines = 0;

char *_getline(const int fd)
{
	char *line = NULL;
	int line_len = 0;
	int i;

	if (fd == -1)
	{
		for (i = 0; i < MAX_FDS; i++)
		{
			read_bytes[i] = 0;
			offset[i] = 0;
			buffer[i][0] = '\0';
		}
		return (NULL);
	}
	if (fd < 0 || fd >= MAX_FDS)
		return (NULL);

	while (1)
	{
		if (offset[fd] >= read_bytes[fd])
		{
			offset[fd] = 0;
			read_bytes[fd] = read(fd, buffer[fd], READ_SIZE);
			if (read_bytes[fd] == 0)
			{
				if (line_len == 0)
					return (NULL);
				break;
			}
			if (read_bytes[fd] == -1)
				return (NULL);
			buffer[fd][read_bytes[fd]] = '\0';
		}
		for (i = offset[fd]; i < read_bytes[fd]; i++)
		{
			if (line_len >= MAX_LINE_LENGTH)
				break;

			line = realloc(line, line_len + 2);
			if (!line)
				return (NULL);

			line[line_len++] = buffer[fd][i];

			if (buffer[fd][i] == '\n')
			{
				line[line_len] = '\0';
				offset[fd] = i + 1;
				total_lines++;
				printf("Total: %d lines\n", total_lines);
				return (line);
			}
		}
		offset[fd] = i;
	}

	if (line_len > 0 && line[line_len - 1] == '\n')
	{
		line[line_len - 1] = '\0';
		line_len--;
	}

	line = realloc(line, line_len + 1);
	if (!line)
		return (NULL);
	line[line_len] = '\0';

	total_lines++;
	printf("Total: %d lines\n", total_lines);

	return (line);
}
