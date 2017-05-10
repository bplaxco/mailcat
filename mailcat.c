#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#define BUF_SIZE 1024 * 4

void converse(int sockfd)
{
	int i, recv_size;
	char buf[BUF_SIZE] = {0};
	char *msgs[] = {
		"220 localhost ESMTP Postfix\n",
		"250 HELO, pleased to meet you\n",
		"250 OK\n",
		"250 OK\n",
		"354 End data with <CR><LF>.<CR><LF>\n",
		"250 OK\n",
		"221 Bye\n"
	};

	for(i = 0; i < sizeof(msgs) / sizeof(char *); i++) {
		send(sockfd, msgs[i], strlen(msgs[i]), 0);
		recv_size = BUF_SIZE;
		while(recv_size == BUF_SIZE) {
			memset(buf, 0, BUF_SIZE);
			recv_size = recv(sockfd, buf, BUF_SIZE, 0);
			printf("%s", buf);
		}
	}
}

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res;
	struct sockaddr_storage client_addr;
	socklen_t addr_size;
	int sockfd, clientfd, status;

	if (argc < 2) {
		printf("Usage: mailcat <port>\n");
		exit(1);
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	status = getaddrinfo(NULL, argv[1], &hints, &res);

	if (status != 0) {
		fprintf(stderr, "getaddrinfo error: %s\n",
			gai_strerror(status));
		exit(1);
	}

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	int enable = 1; /* Reuse socket */
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

	if(bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
		fprintf(stderr, "Address already in use!\n");
		exit(1);
	}

	listen(sockfd, 10);
	clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
	converse(clientfd);
	close(clientfd);
	close(sockfd);

	return 0;
}
