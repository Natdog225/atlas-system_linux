#include "_getline.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

/**
* _getline - Reads an entire line from a file descriptor.
* @fd: The file descriptor to read from.
*
* Return: A pointer to the line, or NULL on failure or EOF.
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
			printf("DEBUG: fd=%d, read_bytes=%d\n", fd, read_bytes[fd]);
			printf("DEBUG: buffer contents: |%s|\n", buffer[fd]);
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
				line = realloc(line, line_len + 1);
				printf("DEBUG: reallocating line to size: %d\n", line_len + 1);
				if (!line)
				{
					perror("DEBUG: realloc failed");
					return (NULL);
				}
				for (int j = 0; j < line_len; j++)

				{
					line[j] = buffer[fd][offset[fd] + j];
				}
				line[line_len] = '\0';
				offset[fd] = i + 1;
				printf("DEBUG: line_len=%d\n", line_len);
				printf("DEBUG: line contents: |%s|\n", line);
				line_len = 0;
				return (line);
			}
		}
		line = realloc(line, line_len + 1);
		printf("DEBUG: reallocating line to size: %d\n", line_len + 1);
		if (!line)
		{
			perror("DEBUG: realloc failed");
			return (NULL);
		}
		for (int j = 0; j < line_len; j++) {

			line[j] = buffer[fd][offset[fd] + j];
		}
		offset[fd] += line_len;
	}
	line = realloc(line, line_len + 1);
	printf("DEBUG: reallocating line to size: %d\n", line_len + 1);
	if (!line)
	{
		perror("DEBUG: realloc failed");
		return (NULL);
	}
	line[line_len] = '\0';
	printf("DEBUG: line_len=%d\n", line_len);
	printf("DEBUG: line contents: |%s|\n", line);
	return (line);
}
