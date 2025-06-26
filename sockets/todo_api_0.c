#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 8192

/**
 * main - Listens on port 8080, accepts connections, and parses HTTP requests for fun.
 *
 * Return: EXIT_FAILURE if an error occurs, otherwise runs forever like life.
 */
int main(void)
{
	int server_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	char buffer[BUFFER_SIZE];
	char method[16], path[256], version[16];
	const char *response = "HTTP/1.1 200 OK\r\n\r\n";

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		perror("socket failed");
		return (EXIT_FAILURE);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

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
	printf("Server listening on port %d\n", PORT);

	while (1)
	{
		client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
		if (client_fd < 0)
		{
			perror("accept failed");
			continue;
		}
		printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
		memset(buffer, 0, BUFFER_SIZE);
		if (recv(client_fd, buffer, BUFFER_SIZE - 1, 0) < 0)
		{
			perror("recv failed");
			close(client_fd);
			continue;
		}
		printf("Raw request: \"%s\"\n", buffer);
		sscanf(buffer, "%s %s %s", method, path, version);
		printf("Method: %s\nPath: %s\nVersion: %s\n", method, path, version);
		send(client_fd, response, strlen(response), 0);
		close(client_fd);
	}
	close(server_fd); /* Unreachable cause who would want to */
	return (EXIT_SUCCESS);
}