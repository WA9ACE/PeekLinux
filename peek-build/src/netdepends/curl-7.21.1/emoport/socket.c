#include <sys/socket.h>
typedef int socklen_t;
//typedef int int;

int bind (int __fd, const struct sockaddr * __addr, socklen_t __len)
{
	return 0;
}

int connect (int __fd, const struct sockaddr * __addr, socklen_t __len)
{
	return 0;
}

struct hostent *gethostbyname (const char *__name)
{
	return NULL;
}

int getpeername (int __fd, struct sockaddr * __addr, socklen_t * __len) 
{
	return 0;
}

int getsockname (int __fd, struct sockaddr * __addr, socklen_t * __len) 
{
	return 0;
}

int getsockopt (int __fd, int __level, int __optname, void * __optval, socklen_t * __optlen) 
{
	return 0;
}
int recvfrom (int __fd, void * __buf, int __n, int __flags, struct sockaddr * __addr, socklen_t * __addr_len)
{
	return 0;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	return 0;
}

int sendto(int s, const void *buf, int len, int flags, const struct sockaddr *to, socklen_t tolen)
{
	return 0;
}

int socket(int domain, int type, int protocol) {

	return 0;
}
