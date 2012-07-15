#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>

#define	MAXLINE	4096
#define	PORT	13
#define BACKLOG	1024

typedef struct sockaddr	SA;


int
main(int argc, char *argv[])
{
	int			listenfd;
	int		 	connectfd;
	struct	sockaddr_in	servaddr;
	struct	sockaddr_in 	cliaddr;
	char			recvline[MAXLINE+1];
	char		 	sendline[MAXLINE];
	char			cliip[MAXLINE];
	int			cliport;
	socklen_t		len;
	time_t			curtime;
	
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("socket error: %d: %s\n",errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0){
		printf("bind error: %d: %s\n",errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (listen(listenfd, BACKLOG) < 0){
		printf("listen error: %d: %s\n",errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	
	memset(&cliaddr, 0, sizeof(cliaddr));

	for(;;){
		len = sizeof(cliaddr);
		if ( (connectfd = accept(listenfd, (SA *)&cliaddr, &len)) < 0){
			if (errno == EINTR)
				continue;
			else{
				printf("accept error: %d: %s\n",errno, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}
		
		memset(recvline, 0, sizeof(recvline));
		if (read(connectfd, recvline, MAXLINE) == EOF){
			continue;
		}

		/*getpeername(connectfd, (SA *)&cliaddr, &len);*/ 
		inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip));
		cliport = ntohs(cliaddr.sin_port);
		printf("%s request form %s port:%d\n", recvline, cliip, cliport);

		curtime = time(NULL);
		memset(sendline, 0, sizeof(sendline));
		snprintf(sendline, sizeof(sendline), "server time is: %s", ctime(&curtime));
		write(connectfd, sendline, sizeof(sendline));
		
		close(connectfd);
	}

	close(listenfd);	/*I think it maybe never to execute.*/
	exit(EXIT_SUCCESS);
}

