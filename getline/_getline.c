#include "_getline.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINES 25

char *_getline(const int fd)
{
	static char buffer[MAX_FDS][READ_SIZE + 1] = {{0}};
	static int bytes_read[MAX_FDS] = {0};
	static int offset[MAX_FDS] = {0};
	static int line_count = 0;
	char *line = NULL;
	int line_len = 0;
	int i, newline_pos;

	if (fd == -1)
	{
		for (i = 0; i < MAX_FDS; i++)
		{
			bytes_read[i] = 0;
			offset[i] = 0;
		}
		line_count = 0;
		return NULL;
	}

	if (fd < 0 || fd >= MAX_FDS || line_count >= MAX_LINES)
	{
		return NULL;
	}

	while (1)
	{
		if (offset[fd] == bytes_read[fd])
		{
			bytes_read[fd] = read(fd, buffer[fd], READ_SIZE);
			offset[fd] = 0;
			if (bytes_read[fd] == -1)
				return (NULL);
			if (bytes_read[fd] == 0 && line_len == 0)
				return NULL;
			if (bytes_read[fd] == 0 && line_len > 0)
				break;
		}

		newline_pos = -1;
		for (i = offset[fd]; i < bytes_read[fd]; i++)
		{
			if (buffer[fd][i] == '\n')
			{
				newline_pos = i;
				break;
			}
		}

		if (newline_pos != -1)
		{
			line = realloc(line, line_len + (newline_pos - offset[fd]) + 1);
			if (!line)
				return NULL;
			memcpy(line + line_len, buffer[fd] + offset[fd], newline_pos - offset[fd]);
			line_len += (newline_pos - offset[fd]);
			line[line_len] = '\0';
			offset[fd] = newline_pos + 1;
			line_count++;
			return line;
		}
		else
		{
			line = realloc(line, line_len + (bytes_read[fd] - offset[fd]));
			if (!line)
				return NULL;
			memcpy(line + line_len, buffer[fd] + offset[fd], bytes_read[fd] - offset[fd]);
			line_len += (bytes_read[fd] - offset[fd]);
			offset[fd] = bytes_read[fd];
		}
	}

	line = realloc(line, line_len + 1);
	if (!line)
		return NULL;
	line[line_len] = '\0';
	line_count++;
	return line;
}
