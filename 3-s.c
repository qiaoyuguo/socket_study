#include "wrapsock.h"

int main(int argc, char* argv[])
{
	int					listenfd, connfd, sockfd;
	struct sockaddr_in	srvaddr, cliaddr;
	char				buf[MAXLINE];
	int					i, n;
	socklen_t			clilen;
	int					maxfd, maxi, nready;
	int					client[FD_SETSIZE];
	fd_set				allset, rset;

	memset(&srvaddr, 0, sizeof(srvaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	Bind(listenfd, (SA*)&srvaddr, sizeof(srvaddr));
	Listen(listenfd, BACKLOG);

	maxfd = listenfd;
	maxi = -1;
	memset(client, -1, sizeof(client));
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);	
	connfd = 0;

	for( ; ; ){
		rset = allset;
		nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

		if( FD_ISSET(listenfd, &rset)){
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (SA*)&cliaddr, &clilen);

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[i] < 0){
					client[i] = connfd;
					break;
				}
			if (i == FD_SETSIZE)
				err_quit("too many clients");
			FD_SET(connfd, &allset);
			if (connfd > maxfd)
				maxfd = connfd;
			if (i > maxi)
				maxi = i;

			if (--nready <= 0)
				continue;
		}
		for (i = 0; i <= maxi; i++){
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)){
				memset(buf, 0, sizeof(buf));
				if ( (n = Read(sockfd, buf, sizeof(buf))) == 0){
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else{
					Fputs(buf, stdout);
					sleep(5);
					Write(sockfd, buf, n);
				}
				if (--nready <= 0)
					break;
			}
		}
	}

	Close(connfd);
	Close(listenfd);
	exit(EXIT_SUCCESS);
}
