#include <sys/socket.h>
#include <sys/time.h>

extern void *p_malloc(int);
typedef int socklen_t;

extern unsigned long bal_gethostbyname(const char *hostname);

int peek_tcp_can_read_sim(int n) 
{ 
	int hasData = SimReadReg();
	emo_printf("SOCKET: peek_tcp_can_read_sim()");
	return hasData; 
}

typedef enum {
  SOCK_CREATE_CNF      =  1, /* Result event of sock_create() */
  SOCK_CLOSE_CNF       =  2, /* Result event of sock_close() */
  SOCK_BIND_CNF        =  3, /* Result event of sock_bind() */
  SOCK_LISTEN_CNF      =  4, /* Result event of sock_listen() */
  SOCK_CONNECT_CNF     =  5, /* Result event of sock_connect() */
  SOCK_SOCKNAME_CNF    =  6, /* Result event of sock_getsockname() */
  SOCK_PEERNAME_CNF    =  7, /* Result event of sock_getpeername() */
  SOCK_HOSTINFO_CNF    =  8, /* Result event of sock_gethostbyname() or sock_gethostbyaddr() */
  SOCK_MTU_SIZE_CNF    =  9, /* Result event of sock_get_mtu_size() */
  SOCK_RECV_IND        = 10, /* Network event: data has been received */
  SOCK_CONNECT_IND     = 11, /* Network event: an incoming connection has been accepted. */
  SOCK_CONN_CLOSED_IND = 12, /* Network event: connection has been closed by the remote peer */
  SOCK_ERROR_IND       = 13, /* Network event: an asynchronous error has occurred */
  SOCK_FLOW_READY_IND  = 14, /* Flow control: the API is ready to send data again */

  SOCK_OPEN_BEARER_CNF,      // Result Event of sock_open_bearer()
  SOCK_CLOSE_BEARER_CNF,     // Result event of sock_close_bearer()
  SOCK_BEARER_INFO_CNF,      // Result event of sock_bearer_info()
  SOCK_BAERER_CLOSED_IND     // The bearer connection has been closed
} T_SOCK_EVENTTYPE;

typedef enum {
  SOCK_RESULT_OK                =  0, /* No problem detected. a corresponding primitive has been sent to the TCP/IP entity */
  SOCK_RESULT_INVALID_PARAMETER =  1, /* A parameter given to the function is invalid */
  SOCK_RESULT_INTERNAL_ERROR    =  2, /* An internal error has happened */
  SOCK_RESULT_ADDR_IN_USE       =  3, /* The address or port is already in use */
  SOCK_RESULT_OUT_OF_MEMORY     =  4, /* There is not enough memory to fulfill the request */
  SOCK_RESULT_NOT_SUPPORTED     =  5, /* The socket is not of a type that can support this operation */
  SOCK_RESULT_UNREACHABLE       =  6, /* The specified host cannot be reached */
  SOCK_RESULT_CONN_REFUSED      =  7, /* The connection to the specified address was refused by the remote host */
  SOCK_RESULT_TIMEOUT           =  8, /* The connection attempt timed out without establishing a connection */
  SOCK_RESULT_IS_CONNECTED      =  9, /* The request could not be fulfilled because the socket is already connected */
  SOCK_RESULT_HOST_NOT_FOUND    = 10, /* The specified host could not be found in the DNS */
  SOCK_RESULT_DNS_TEMP_ERROR    = 11, /* A temporary DNS error has occurred. Retrying the query may be successful */
  SOCK_RESULT_DNS_PERM_ERROR    = 12, /* A permanent DNS error has occurred */
  SOCK_RESULT_NO_IPADDR         = 13, /* The specified name has been found in the DNS, but no IP address is available */
  SOCK_RESULT_NOT_CONNECTED     = 14, /* The socket has not been connected yet */
  SOCK_RESULT_MSG_TOO_BIG       = 15, /* The size of the data buffer is too large for a UDP socket */
  SOCK_RESULT_CONN_RESET        = 16, /* The connection has been reset by the remote peer */
  SOCK_RESULT_CONN_ABORTED      = 17, /* The connection was aborted due to timeout or some other error condition */
  SOCK_RESULT_NO_BUFSPACE       = 18, /* Sending failed temporarily because the space to buffer the message was exhausted. */
  SOCK_RESULT_NETWORK_LOST, //  As a result code: The operation failed because TCP/IP's bearer connection has been disconnected.As an asynchronous event code: The bearer connection has been closed.
  SOCK_RESULT_NOT_READY, //     The operation failed because the bearer connection has not been opened.
  SOCK_RESULT_BEARER_NOT_READY, //      The bearer connection could not be opened because the mobile is not yet completely attached to the network. A retry at a later time may be successful.
  SOCK_RESULT_IN_PROGRESS, //   The operation failed because a similar operation is already in progress.
  SOCK_RESULT_BEARER_ACTIVE//   The operation failed because a bearer connection is already open.
} T_SOCK_RESULT;

int bind_sim(int fd, const struct sockaddr * addr, socklen_t len)
{
	emo_printf("SOCKET bind(fd=%d, addr=%08X, len=%d)", fd, addr, len);
	return 0;
}

struct hostent *gethostbyname_sim(const char *name)
{
	emo_printf("SOCKET gethostbyname(name=%s)", name);
	return (struct hostent *)bal_gethostbyname(name);
}

int socket_sim(int domain, int type, int protocol) 
{
	int ret;
	emo_printf("SOCKET socket(domain=%d, type=%d, protocol=%d)", domain, type, protocol);
	ret = bal_socket(0 /*AF_INET*/, 0 /*SOCK_STREAM*/, 0 /*IPPROTO_TCP*/);

	emo_printf("SOCKET socket() returned %d", ret);

	return ret;
}

int connect_sim(int fd, const struct sockaddr * addr, socklen_t len)
{
	struct sockaddr_in *sockin = (struct sockaddr_in *)addr;
	char host[32];
	char *ipaddr = (char *)((unsigned int *)(&sockin->sin_addr));
	int ret;

	emo_printf("SOCKET connect(fd=%d, addr=%08X, len=%d)", fd, addr, len);

	ret = bal_connect(fd, addr, len);

	emo_printf("SOCKET connect() returned %d", ret);

	return ret;
}

int select_sim(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int n;
	int hasData = 0;

	emo_printf("SOCKET select(fd=%d, readfds=%08X, writefds=%08X, exceptfds=%08X, timeout=%08X)", nfds, readfds, writefds, exceptfds, timeout);
	for (n = 0; n < nfds; ++n)
	{
		if (FD_ISSET(n, readfds))
			if (!peek_tcp_can_read_sim(n)) // XXX this is the context hack here
				FD_CLR(n, readfds);
			else
				hasData++;
	}

	if (!hasData)
		TCCE_Task_Sleep(timeout->tv_sec * 1000 + timeout->tv_usec / 1000);
	
	emo_printf("SOCKET select() returns %d", hasData);
	return hasData;
}

int send_sim(int fd, const void *buf, int len, int flags)
{
	int ret;

	emo_printf("SOCKET sendto(s=%d, buf=%08X, len=%d, flags=%d)", fd, buf, len, flags);

	ret = bal_send(fd, buf, len, 0);

	emo_printf("SOCKET sendto() returned %d", ret);

	return ret;
}

int recv_sim(int fd, void * buf, int n, int flags)
{
	int ret;

	emo_printf("SOCKET recvfrom(fd=%d, buf=%08X, n=%d, flags=%d)", fd, buf, n, flags); //, addr, *addr_len);

	ret = bal_recv(fd, buf, n, 0);	

	emo_printf("SOCKET recvfrom() returned %d", ret);

	return ret;
}

