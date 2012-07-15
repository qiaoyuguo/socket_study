#include "wrapsock.h"
#include <sys/epoll.h>

int main(int argc, char* argv[])
{
	int		listenfd;
	struct	sockaddr_in srvaddr;
	char*	recvline[MAXLINE];

	memset(recvline, 0, MAXLINE);

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	Bind(listenfd, (SA*)&srvaddr, sizeof(srvaddr));
	Listen(listenfd, BACKLOG);

	exit(EXIT_SUCCESS);
}
