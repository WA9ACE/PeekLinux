#include "TCPTransport.h"

#include "p_malloc.h"

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#ifdef WIN32
#include <Winsock2.h>
#include <ws2tcpip.h>
#define ioctl ioctlsocket
#else
#define closesocket close
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#ifdef LINUX
#include <fcntl.h>
#include <asm/ioctls.h>
#endif
#endif

Endpoint *TCPSocket(void);
int TCPConnect(Endpoint *ep, URL *destination);
int TCPListen(Endpoint *ep, int backlog);
int TCPBind(Endpoint *ep, URL *bindaddr);
Endpoint *TCPAccept(Endpoint *ep);
int TCPWrite(Endpoint *ep, const void *buffer, size_t len);
int TCPRead(Endpoint *ep, void *output, size_t len);
int TCPPeek(Endpoint *ep, void *output, size_t len);
int TCPClose(Endpoint *ep);
void TCPFree(Endpoint *ep);
unsigned int TCPSequenceID(Endpoint *ep);

Transport TCPTransport = {TCPSocket, TCPConnect, TCPListen, TCPBind,
		TCPAccept, TCPWrite, TCPRead, TCPPeek, TCPClose, TCPFree,
		TCPSequenceID};

struct TCPEndpoint_t {
	Transport *transport;
	int fd;
	unsigned int sequenceID;
};
typedef struct TCPEndpoint_t TCPEndpoint;

static int TCPInit(void);

Endpoint *TCPSocket(void)
{
	TCPEndpoint *ep;
	int on;

	TCPInit();
	
	ep = (TCPEndpoint *)p_malloc(sizeof(TCPEndpoint));
	ep->transport = (Transport *)&TCPTransport;
	ep->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ep->fd == -1) {
		p_free(ep);
		return NULL;
	}

	on = 1;
#ifdef LINUX
	fcntl(ep->fd, F_SETFL, O_NONBLOCK);
#else
	ioctl(ep->fd, FIONBIO, (unsigned long *)&on);
#endif

	return (Endpoint *)ep;
}

int TCPConnect(Endpoint *iep, URL *destination)
{
	TCPEndpoint *ep;
	unsigned short port;
	struct in_addr addr;
	struct sockaddr_in connect_in;
	struct hostent *remoteHost;
	int on;

	if (destination == NULL || destination->port == NULL || destination->hostname == NULL)
		return -1;

	ep = (TCPEndpoint *)iep;

	port = atoi(destination->port);

	if (isalpha(destination->hostname[0])) {
		remoteHost = gethostbyname(destination->hostname);
	} else {
		addr.s_addr = inet_addr(destination->hostname);
        if (addr.s_addr == INADDR_NONE) {
			closesocket(ep->fd);
			p_free(ep);
            return -1;
		} else {
            remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);
		}
	}

	if (remoteHost == NULL) {
		closesocket(ep->fd);
		p_free(ep);
        return -1;
	}

	memset(&connect_in, 0, sizeof(struct sockaddr_in));
	connect_in.sin_family = AF_INET;
	connect_in.sin_port = htons(port);
	connect_in.sin_addr = *((struct in_addr *)remoteHost->h_addr_list[0]);

	on = 0;
#ifdef LINUX
	fcntl(ep->fd, F_SETFL, 0);
#else
	ioctl(ep->fd, FIONBIO, (unsigned long *)&on);
#endif

	if (connect(ep->fd, (const struct sockaddr *)&connect_in,
			sizeof(struct sockaddr_in)) == -1) {
		closesocket(ep->fd);
		p_free(ep);
        return -1;
	}

	on = 1;
#ifdef LINUX
	fcntl(ep->fd, F_SETFL, O_NONBLOCK);
#else
	ioctl(ep->fd, FIONBIO, (unsigned long *)&on);
#endif

	ep->sequenceID = 1;

	return 0;
}

int TCPListen(Endpoint *ep, int backlog)
{
	TCPEndpoint *tep;
	int on;
	
	if (ep == NULL)
		return -1;

	tep = (TCPEndpoint *)ep;
	on = 1;
	setsockopt(tep->fd, SOL_SOCKET,  SO_REUSEADDR,
			(char *)&on, sizeof(on));

	return listen(tep->fd, backlog);
}

int TCPBind(Endpoint *ep, URL *bindaddr)
{
	TCPEndpoint *tep;
	unsigned short port;
	struct in_addr addr;
	struct sockaddr_in bind_in;
    int optval;

	if (ep == NULL || bindaddr == NULL || bindaddr->hostname == NULL
			|| bindaddr->port == NULL)
		return -1;

	tep = (TCPEndpoint *)ep;

    fprintf(stderr, "Binding to '%s' '%s'\n", bindaddr->hostname, bindaddr->port);

	port = atoi(bindaddr->port);

	if (tep->fd == -1) {
		return -1;
	}

	addr.s_addr = inet_addr(bindaddr->hostname);
    if (addr.s_addr == INADDR_NONE) {
		return -1;
	}

	memset(&bind_in, 0, sizeof(struct sockaddr_in));
	bind_in.sin_family = AF_INET;
	bind_in.sin_port = htons(port);
	bind_in.sin_addr = addr;

    optval = 1;
    setsockopt(tep->fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, sizeof(int));

	return bind(tep->fd, (struct sockaddr *)&bind_in, sizeof(struct sockaddr_in));
}

Endpoint *TCPAccept(Endpoint *ep)
{
	TCPEndpoint *tep, *output;

	if (ep == NULL)
		return NULL;

	tep = (TCPEndpoint *)ep;

	output = (TCPEndpoint *)p_malloc(sizeof(TCPEndpoint));
	if (output == NULL)
		return NULL;
	output->fd = accept(tep->fd, NULL, NULL);
	if (output->fd == -1) {
		p_free(output);
		return NULL;
	}

	output->sequenceID = 2;

	output->transport = &TCPTransport;
	return (Endpoint *)output;
}

int TCPWrite(Endpoint *ep, const void *buffer, size_t len)
{
	TCPEndpoint *tep;

	if (ep == NULL || buffer == NULL)
		return -1;

	if (len == 0)
		return 0;

	tep = (TCPEndpoint *)ep;
	/*fprintf(stderr, "Sending %d bytes\n", len);*/
	return send(tep->fd, buffer, len, 0);
}

int TCPRead(Endpoint *ep, void *output, size_t len)
{
	TCPEndpoint *tep;

	if (ep == NULL || output == NULL)
		return -1;

	if (len == 0)
		return 0;

	tep = (TCPEndpoint *)ep;
	return recv(tep->fd, output, len, 0);
}

int TCPPeek(Endpoint *ep, void *output, size_t len)
{
	TCPEndpoint *tep;
	int retval;
	int errval;

	if (ep == NULL || output == NULL)
		return -1;

	tep = (TCPEndpoint *)ep;
	retval = recv(tep->fd, output, len, MSG_PEEK
#ifdef LINUX
		| MSG_DONTWAIT
#endif
		);

	if (retval < 0) {
#ifdef WIN32
		errval = WSAGetLastError();
		if (errval != WSAEWOULDBLOCK)
			fprintf(stderr, "WinsockError: %d\n", errval);
#else
		/*fprintf(stderr, "Socket error: %d %s\n", retval, strerror(errno));*/
#endif
	}

	return retval;
}

int TCPClose(Endpoint *ep)
{
    TCPEndpoint *tep;

    if (ep == NULL)
        return -1;

    tep = (TCPEndpoint *)ep;
    return closesocket(tep->fd);
}

void TCPFree(Endpoint *ep)
{
    p_free(ep);
}

unsigned int TCPSequenceID(Endpoint *ep)
{
	TCPEndpoint *tep;
	tep = (TCPEndpoint *)ep;
	tep->sequenceID += 2;
	return tep->sequenceID - 2;
}

static int TCPInit(void)
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	static int initDone = 0;
	 
	if (initDone == 0) {
		wVersionRequested = MAKEWORD( 2, 2 );
		 
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			return 0;
		}
		 
		/* Confirm that the WinSock DLL supports 2.2.*/
		/* Note that if the DLL supports versions greater    */
		/* than 2.2 in addition to 2.2, it will still return */
		/* 2.2 in wVersion since that is the version we      */
		/* requested.                                        */
		 
		if ( LOBYTE( wsaData.wVersion ) != 2 ||
				HIBYTE( wsaData.wVersion ) != 2 ) {
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			WSACleanup( );
			return 0; 
		}
		 
		/* The WinSock DLL is acceptable. Proceed. */
		initDone = 1;
	}
#endif
	return 1;
}
