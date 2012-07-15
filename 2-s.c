#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#define	MAXLINE	4096
#define	PORT	13
#define BACKLOG	1024

typedef struct sockaddr	SA;

void echo_time(int connectfd);
void sig_chld(int signo);

	int
main(int argc, char *argv[])
{
	int					listenfd;
	int					connectfd;
	struct	sockaddr_in	servaddr;
	pid_t				childpid;

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

	signal (SIGCHLD, sig_chld); /*  must call waitpid() */

	for(;;){
		if ( (connectfd = accept(listenfd, NULL, NULL)) < 0){
			if (errno == EINTR)
				continue;
			else{
				printf("accept error: %d: %s\n",errno, strerror(errno));
				exit(EXIT_FAILURE);
			}
		}

		if ( (childpid = fork()) == 0){
			close(listenfd);
			echo_time(connectfd);
			exit(0);
		}
		close(connectfd);
	}

	close(listenfd);	/*I think it maybe never to execute.*/
	exit(EXIT_SUCCESS);
}

	void
echo_time(int connectfd)
{

	char					recvline[MAXLINE+1];
	int						cliport;
	struct	sockaddr_in 	cliaddr;
	time_t					curtime;
	char					sendline[MAXLINE];
	char					cliip[MAXLINE];
	int						n;
	socklen_t				clilen;

	memset(&cliaddr, 0, sizeof(cliaddr));
	memset(recvline, 0, sizeof(recvline));
	clilen = sizeof(cliaddr);
again:
	while ( (n = read(connectfd, recvline, MAXLINE) > 0)){
		getpeername(connectfd, (SA *)&cliaddr, &clilen);
		inet_ntop(AF_INET, &cliaddr.sin_addr, cliip, sizeof(cliip));
		cliport = ntohs(cliaddr.sin_port);
		printf("%s request form %s port:%d\n", recvline, cliip, cliport);

		curtime = time(NULL);
		memset(sendline, 0, sizeof(sendline));
		snprintf(sendline, sizeof(sendline), "server time is: %s", ctime(&curtime));
		write(connectfd, sendline, sizeof(sendline));
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0){
		printf("echo_time error:%d: %s\n",errno,strerror(errno));
		exit(EXIT_FAILURE);
	}

	close(connectfd);
	exit(EXIT_SUCCESS);
}

	void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( ( pid = waitpid(-1, &stat, WNOHANG) > 0))
		printf("child %d terminated\n", pid);
	return;
}
