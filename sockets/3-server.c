#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

/**
 * main - Accepts a connection, reads a message, and prints it.
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	char buffer[1024] = {0};

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket failed");
		return (EXIT_FAILURE);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(12345);

	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("bind failed");
		close(server_fd);
		return (EXIT_FAILURE);
	}

	if (listen(server_fd, 10) < 0)
	{
		perror("listen failed");
		close(server_fd);
		return (EXIT_FAILURE);
	}

	printf("Server listening on port 12345\n");
	client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_fd < 0)
	{
		perror("accept failed");
		close(server_fd);
		return (EXIT_FAILURE);
	}
	printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));

	/* 6. Read message from client */
	if (recv(client_fd, buffer, sizeof(buffer) - 1, 0) < 0)
	{
		perror("recv failed");
	}
	else
	{
		printf("Message received: \"%s\"\n", buffer);
	}

	close(client_fd);
	close(server_fd);

	return (EXIT_SUCCESS);
}
