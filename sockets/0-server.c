#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

/**
 * main - Opens an IPv4/TCP socket and listens on port 12345.
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int main(void)
{
	int server_fd;
	struct sockaddr_in server_addr;

	/* 1. Create the socket */
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket failed");
		return (EXIT_FAILURE);
	}

	/* 2. Set up the address structure */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; /* Listen on any address */
	server_addr.sin_port = htons(12345); /* Port in network byte order */

	/* 3. Bind the socket to the address and port */
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind failed");
		close(server_fd);
		return (EXIT_FAILURE);
	}

	/* 4. Listen for incoming connections */
	if (listen(server_fd, 10) < 0)
	{
		perror("listen failed");
		close(server_fd);
		return (EXIT_FAILURE);
	}

	printf("Server listening on port 12345\n");

	/* Hang indefinitely */
	pause();

	/* This part is useless cause you never get here currently */
	close(server_fd);
	return (EXIT_SUCCESS);
}