#include "wrapsock.h"

#define	max(a,b)	((a) > (b) ? (a) : (b))

	int
main(int argc, char* argv[])
{
	int					sockfd;
	struct sockaddr_in	srvaddr;
	char				send[MAXLINE], recv[MAXLINE];
	fd_set				rset;
	int					maxfdp1;
	int					stdineof;

	memset(&srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, argv[1], &srvaddr.sin_addr) <= 0)
		err_quit("inet_pton error");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	Connect(sockfd, (SA*)&srvaddr, sizeof(srvaddr));

	FD_ZERO(&rset);
	stdineof = 0;
	for ( ; ; ){
		FD_SET(fileno(stdin), &rset);
		FD_SET(sockfd, &rset);
		maxfdp1 = max(fileno(stdin), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		memset(recv, 0, sizeof(recv));
		if (FD_ISSET(sockfd, &rset))
			if ( Read(sockfd, recv, sizeof(recv)) == 0){
				if (stdineof == 1)
					return 0;
			}
		
		Fputs(recv, stdout);

		memset(send, 0, sizeof(send));
		if (FD_ISSET(fileno(stdin), &rset)){
			if (Fgets(send, sizeof(send), stdin) == NULL){
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR);
				FD_CLR(fileno(stdin), &rset);
				continue;
			}
			Write(sockfd, send, strlen(send));
		}
	}

	Close(sockfd);
	exit(EXIT_SUCCESS);
}
