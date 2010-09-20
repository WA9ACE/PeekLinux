#include <sys/socket.h>
#include <sys/time.h>
#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "p_sim.h"

#define _ENTITY_PREFIXED(N) ui_##N

#define hCommAPP _ENTITY_PREFIXED(hCommAPP)

#ifdef TI_PS_HCOMM_CHANGE
#define PSENDX(A,B) PSEND(_hComm##A,B)
#else
#define PSENDX(A,B) PSEND(hComm##A,B)
#endif 

extern T_HANDLE hCommAPP;

extern void *p_malloc(int);

typedef int socklen_t;

extern void peek_obtain_socket_semaphore();
extern int peek_tcp_get_socket_result();

extern void peek_obtain_connect_semaphore();
extern int peek_tcp_get_connect_result();

extern void peek_obtain_write_semaphore();
extern int peek_tcp_get_send_result();

extern int peek_tcp_can_read(int fd);
extern void peek_sleep(int ms);

int bind(int fd, const struct sockaddr * addr, socklen_t len)
{
  emo_printf("SOCKET bind(fd=%d, addr=%08X, len=%d)", fd, addr, len);
  return 0;
}

struct hostent *gethostbyname(const char *name)
{
  emo_printf("SOCKET gethostbyname(name=%s)", name);
  return NULL;
}

int getpeername(int fd, struct sockaddr * addr, socklen_t * len) 
{
  emo_printf("SOCKET getpeername(fd=%d, addr=%08X, len=%d)", fd, addr, *len);
	return 0;
}

int getsockname(int fd, struct sockaddr * addr, socklen_t * len) 
{
  emo_printf("SOCKET getsockname(fd=%d, addr=%08X, len=%d)", fd, addr, *len);
	return 0;
}

int getsockopt(int fd, int level, int optname, void * optval, socklen_t * optlen) 
{
  emo_printf("SOCKET getsockopt(fd=%d, level=%d, optname=%d, optval=%08X, optlen=%d)", fd, level, optname, optval, *optlen);
	return 0;
}
int recvfrom(int fd, void * buf, int n, int flags, struct sockaddr * addr, socklen_t * addr_len)
{
  emo_printf("SOCKET recvfrom(fd=%d, buf=%08X, n=%d, flags=%d, addr=%08X, addr_len=%d)", fd, buf, n, flags, addr, *addr_len);
  return peek_tcp_read(fd, buf, n);
}

int socket(int domain, int type, int protocol) 
{
	T_EMOBIIX_NETSURF_SOCKET *msg;
	int ret;

  emo_printf("SOCKET socket(domain=%d, type=%d, protocol=%d)", domain, type, protocol);

	msg = P_ALLOC(EMOBIIX_NETSURF_SOCKET);
	msg->protocol = 0;
	PSENDX(APP, msg);

	peek_obtain_socket_semaphore();

	ret = peek_tcp_get_socket_result();

	emo_printf("SOCKET socket() returned %d", ret);

	return ret;
}

int connect(int fd, const struct sockaddr * addr, socklen_t len)
{
	T_EMOBIIX_NETSURF_CONNECT *msg;
	struct sockaddr_in *sockin = (struct sockaddr_in *)addr;
	char host[32];
	char *ipaddr = (char *)((unsigned int *)(&sockin->sin_addr));
	int ret;

    emo_printf("SOCKET connect(fd=%d, addr=%08X, len=%d)", fd, addr, len);

	sprintf(host, "%d.%d.%d.%d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]);

	msg = P_ALLOC(EMOBIIX_NETSURF_CONNECT);
	msg->fd = fd;
	strcpy(msg->host, host);
	msg->port = ntohs(sockin->sin_port);
	PSENDX(APP, msg);

	peek_obtain_connect_semaphore();

	ret = peek_tcp_get_connect_result();

	emo_printf("SOCKET connect() returned %d", ret);

	return ret;
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int n;
	int hasData = 0;

  emo_printf("SOCKET select(fd=%d, readfds=%08X, writefds=%08X, exceptfds=%08X, timeout=%08X)", nfds, readfds, writefds, exceptfds, timeout);
	for (n = 0; n < nfds; ++n)
	{
		if (FD_ISSET(n, readfds))
			if (!peek_tcp_can_read(n))
				FD_CLR(n, readfds);
			else
				hasData++;
	}

	if (!hasData)
		TCCE_Task_Sleep(timeout->tv_sec * 1000 + timeout->tv_usec / 1000);

	emo_printf("SOCKET select() returns %d", hasData);
	return hasData;
}

int sendto(int s, const void *buf, int len, int flags, const struct sockaddr *to, socklen_t tolen)
{
	T_EMOBIIX_NETSURF_SEND *msg;

	emo_printf("SOCKET sendto(s=%d, buf=%08X, len=%d, flags=%d, to=%08X, tolen=%d)", s, buf, len, flags, to, tolen);

	msg = P_ALLOC(EMOBIIX_NETSURF_SEND);
	msg->fd = s;
	msg->buf = p_malloc(len);
	memcpy(msg->buf, buf, len);
	msg->len = len;
	PSENDX(APP, msg);

	peek_obtain_write_semaphore();

	return peek_tcp_get_send_result();
}

