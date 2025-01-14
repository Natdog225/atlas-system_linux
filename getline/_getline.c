#include "_getline.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/**
 * _getline - Reads an entire line from a file descriptor.
 * @fd: The file descriptor to read from.
 *
 * Return: A null-terminated string that does not include the newline
 *         character, or NULL if there are no more lines or an error occurs.
 */
char *_getline(const int fd)
{
	static char buffer[MAX_FDS][READ_SIZE + 1];
	static int bytes_read[MAX_FDS];
	static int offset[MAX_FDS];
	char *line = NULL;
	int line_len = 0;
	int i, j;

	/* Check for invalid file descriptor */
	if (fd < 0 || fd >= MAX_FDS)
		return (NULL);

	while (1)
	{
		/* Read more data if the buffer is empty */
		if (offset[fd] >= bytes_read[fd])
		{
			bytes_read[fd] = read(fd, buffer[fd], READ_SIZE);
			offset[fd] = 0;

			/* Handle end of file or errors */
			if (bytes_read[fd] <= 0)
			{
				if (line_len > 0)
				{
					line[line_len] = '\0';
					return (line);
				}
				return (NULL);
			}
		}

		/* Search for newline character */
		for (i = offset[fd]; i < bytes_read[fd]; i++)
		{
			if (buffer[fd][i] == '\n')
			{
				/* Allocate memory for the line */
				line = realloc(line, line_len + i - offset[fd] + 1);
				if (!line)
					return (NULL);

				/* Copy the line from the buffer */
				for (j = 0; j < i - offset[fd]; j++)
					line[line_len + j] = buffer[fd][offset[fd] + j];

				line_len += i - offset[fd];
				line[line_len] = '\0';
				offset[fd] = i + 1;
				return (line);
			}
		}

		/* If no newline is found, append the entire buffer to the line */
		line = realloc(line, line_len + bytes_read[fd] - offset[fd] + 1);
		if (!line)
			return (NULL);

		for (j = 0; j < bytes_read[fd] - offset[fd]; j++)
			line[line_len + j] = buffer[fd][offset[fd] + j];

		line_len += bytes_read[fd] - offset[fd];
		offset[fd] = bytes_read[fd];
	}

	return (NULL);
}
