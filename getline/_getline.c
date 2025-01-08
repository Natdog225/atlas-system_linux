#include "_getline.h"
#include <stdlib.h>
#include <unistd.h>

/**
 * _getline - Reads an entire line from a file descriptor.
 * @fd: The file descriptor to read from.
 *
 * Return: A pointer to the line read, or NULL on failure or EOF.
 */
char *_getline(const int fd)
{
	static char buffer[MAX_FDS][READ_SIZE + 1] = {{0}};
	static int read_bytes[MAX_FDS] = {0};
	static int offset[MAX_FDS] = {0};
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
			line_len++;
			if (buffer[fd][i] == '\n')
			{
				line = realloc(line, line_len + 2);
				if (!line)
					return (NULL);
				line[line_len] = '\n';
				line[line_len - 1] = '\0';
				offset[fd] = i + 1;
				return (line);
			}
			line_len++;
		}
		line = realloc(line, line_len + 1);
		if (!line)
			return (NULL);
		for (i = offset[fd]; i < read_bytes[fd]; i++)
		{
			line[line_len - (read_bytes[fd] - i)] = buffer[fd][i];
		}
		offset[fd] = i;
	}
	line = realloc(line, line_len + 1);
	if (!line)
		return (NULL);
	line[line_len] = '\0';
	return (line);
}
