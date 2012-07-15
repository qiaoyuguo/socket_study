
#include <stdlib.h>
#include <netinet/in.h> 
#include <sys/socket.h>
#include <sys/types.h> 
//#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>	/*for unix system call read()*/


#define	MAXLINE		4096
#define	PORT		13


typedef struct sockaddr	SA;

	int
main(int argc, char *argv[])
{
	int			sockfd;
	int		 	n;
	char			recvline[MAXLINE + 1];
	char			sendline[MAXLINE];
	struct	sockaddr_in	servaddr;

	if (argc != 2){
		printf("usage: a.out <IPadress>\n");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		printf("inet_pton error for %s\n : %d: %s\n", 
				argv[1], errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	for(;;){
		if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			printf("socket error: %d: %s\n",errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0){
			printf("connect error: %d: %s\n",errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
		snprintf(sendline, sizeof(sendline), "what's the time?");
		if (write(sockfd, sendline, sizeof(sendline)) < 0){
			printf("write error: %d: %s\n",errno, strerror(errno));
			exit(EXIT_FAILURE);
		}
		if ( (n = read(sockfd, recvline, MAXLINE)) > 0){
			recvline[n] = 0;
			printf("%s", recvline);
		}
		close(sockfd);
		sleep(1);
	}

	exit(EXIT_SUCCESS);
}

