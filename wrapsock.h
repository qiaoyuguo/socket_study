#ifndef __wrapsock_h
#define __wrapsock_h

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>


#define	BACKLOG		1024
#define	PORT		5000
#define MAXLINE		4096


typedef	struct sockaddr	SA;

void	err_quit(char* msg);

int		Socket(int family, int type, int protocol);

void	Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void	Listen(int fd, int backlog);

int		Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

void	Connect(int fd, const struct sockaddr *sa, socklen_t salen);

int		Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

void	Shutdown(int fd, int how);

void	Fputs(const char *ptr, FILE *stream);

char*	Fgets(char *ptr, int n, FILE *stream);

ssize_t	Read(int fd, void *buf, size_t count);

ssize_t Write(int fd, const void *buf, size_t count);

int		Close(int fd);


#endif /* __wrapsock_h */
