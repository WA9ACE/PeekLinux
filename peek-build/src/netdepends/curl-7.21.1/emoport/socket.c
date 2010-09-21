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

extern void peek_obtain_dns_semaphore();
extern unsigned int peek_tcp_get_dns_result();

extern int peek_tcp_can_read(int fd);
extern void peek_sleep(int ms);

int bind(int fd, const struct sockaddr * addr, socklen_t len)
{
//  emo_printf("SOCKET bind(fd=%d, addr=%08X, len=%d)", fd, addr, len);
  return 0;
}

struct hostent *gethostbyname(const char *name)
{
	static unsigned int addr_ip;
    static char *addr_ip_list[2] = { (char *)&addr_ip, NULL };
    static char addr_name[256];
    static struct hostent entry;

	emo_printf("SOCKET gethostbyname(name=%s)", name);

	{ // take taht piece of shit c-style declares ;)
		T_EMOBIIX_NETSURF_DNS *msg;
		msg = P_ALLOC(EMOBIIX_NETSURF_DNS);
		msg->buf = p_malloc(strlen(name) + 1);
		strcpy(msg->buf, name);
		PSENDX(APP, msg);
	}

	peek_obtain_dns_semaphore();

	addr_ip = peek_tcp_get_dns_result();

    emo_printf("SOCKET gethostbyname() returned addr_in %d.%d.%d.%d", ((char *)(&addr_ip))[0], ((char *)(&addr_ip))[1], ((char *)(&addr_ip))[2], ((char *)(&addr_ip))[3]);
    strncpy(addr_name, name, 256);

	if (!addr_ip)
		return NULL;

    memset(&entry, 0, sizeof(entry));
    entry.h_name = addr_name;
    entry.h_addr_list = addr_ip_list;
    entry.h_addrtype = 2; /*AF_INET */

	return &entry;
}

int getpeername(int fd, struct sockaddr * addr, socklen_t * len) 
{
//  emo_printf("SOCKET getpeername(fd=%d, addr=%08X, len=%d)", fd, addr, *len);
	return 0;
}

int getsockname(int fd, struct sockaddr * addr, socklen_t * len) 
{
//  emo_printf("SOCKET getsockname(fd=%d, addr=%08X, len=%d)", fd, addr, *len);
	return 0;
}

int getsockopt(int fd, int level, int optname, void * optval, socklen_t * optlen) 
{
 // emo_printf("SOCKET getsockopt(fd=%d, level=%d, optname=%d, optval=%08X, optlen=%d)", fd, level, optname, optval, *optlen);
	return 0;
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

	emo_printf("SOCKET select(fd=%d)", nfds); 
	for (n = 0; n < nfds; ++n)
	{
		if (FD_ISSET(n, readfds))
			if (!peek_tcp_can_read(n))
				FD_CLR(n, readfds);
			else
				hasData++;
	}

	if (!hasData && timeout)
		TCCE_Task_Sleep(timeout->tv_sec * 1000 + timeout->tv_usec / 1000);

	emo_printf("SOCKET select() returned %d", hasData);

	return hasData;
}

int send(int s, const void *buf, unsigned int len, int flags)
{
	T_EMOBIIX_NETSURF_SEND *msg;
	int ret;

	emo_printf("SOCKET send(s=%d, buf=%08X, len=%d, flags=%d)", s, buf, len, flags);

	msg = P_ALLOC(EMOBIIX_NETSURF_SEND);
	msg->fd = s;
	msg->buf = p_malloc(len);
	if(!msg->buf)  {
		emo_printf("send() failed to malloc buffer");
		return -1;
	}
	memcpy(msg->buf, buf, len);
	msg->len = len;
	PSENDX(APP, msg);

	peek_obtain_write_semaphore();

	ret = peek_tcp_get_send_result();

	emo_printf("SOCKET send() returned %d", ret);

	return ret;
}

int recv(int fd, void * buf, int n, int flags)
{
	T_EMOBIIX_SOCK_RECV *msg;
	int ret;

	emo_printf("SOCKET recv(fd=%d, buf=%08X, n=%d, flags=%d)", fd, buf, n, flags);

	ret = peek_tcp_read(fd, buf, n);

	emo_printf("SOCKET recv() returned %d", ret);

	msg = P_ALLOC(EMOBIIX_SOCK_RECV);
    msg->data = fd;
	PSENDX(APP, msg);

	return ret;
}

