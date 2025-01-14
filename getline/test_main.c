#include <stdio.h>
#include <fcntl.h>
#include "_getline.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	int fd;
	char *line;

	// Test newline_only.txt
	fd = open("newline_only.txt", O_RDONLY);
	if (fd == -1)
	{
		perror("Error opening newline_only.txt");
		return 1;
	}
	printf("Testing newline_only.txt:\n");
	while ((line = _getline(fd)))
	{
		printf("Line: |%s| (Length: %lu)\n", line, strlen(line));
		free(line);
	}
	close(fd);

	// Test multiple_newlines.txt
	fd = open("multiple_newlines.txt", O_RDONLY);
	if (fd == -1)
	{
		perror("Error opening multiple_newlines.txt");
		return 1;
	}
	printf("\nTesting multiple_newlines.txt:\n");
	while ((line = _getline(fd)))
	{
		printf("Line: |%s| (Length: %lu)\n", line, strlen(line));
		free(line);
	}
	close(fd);

	// Test one_char.txt
	fd = open("one_char.txt", O_RDONLY);
	if (fd == -1)
	{
		perror("Error opening one_char.txt");
		return 1;
	}
	printf("\nTesting one_char.txt:\n");
	while ((line = _getline(fd)))
	{
		printf("Line: |%s| (Length: %lu)\n", line, strlen(line));
		free(line);
	}
	close(fd);

	return 0;
}
