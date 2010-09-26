#include "nucleus.h"
#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "socket_api.h"
#include "p_sim.h"
#include "app.h"

extern int errno;

#define EAGAIN          11      /* Try again */

extern int app_socket(PROC_CONTEXT_T* pcont, int type, int protocol);
extern int app_connect(PROC_CONTEXT_T *pcont, const char *host, int port);

extern void app_connect_info(const char *server, unsigned short port);
extern int app_socket(PROC_CONTEXT_T *pcont, int __type, int __protocol);
extern BOOL app_send_buf(PROC_CONTEXT_T *pcont, char *buffer, int size);
extern void app_switch_flow(PROC_CONTEXT_T *pcont, int flow_on);
extern void app_close_tcp_bearer(PROC_CONTEXT_T *context);
extern void app_gethostbyname(const char *hostname);
void peek_tcp_enable_read(int fd);

#define MAX_TCP_SOCKETS 	30
static PROC_CONTEXT_T s_tcp_sockets[MAX_TCP_SOCKETS];

static NU_SEMAPHORE mutex_socket;
//static NU_SEMAPHORE mutex_connect;
static NU_SEMAPHORE mutex_write;
static NU_SEMAPHORE mutex_close;
static NU_SEMAPHORE mutex_dns;

static int s_tcp_fd = -1;
int peek_tcp_get_socket_result()
{
	return s_tcp_fd;
}

static int s_tcp_send_res = -1;
int peek_tcp_get_send_result()
{
	return s_tcp_send_res;
}

static unsigned int s_tcp_dns_res = 0;
unsigned int peek_tcp_get_dns_result()
{
	return s_tcp_dns_res;
}

static char *s_read_data = NULL;

void peek_initialize_sockets()
{
	NU_Create_Semaphore(&mutex_socket, "peek_tcp_socket", 0, NU_PRIORITY);
	NU_Create_Semaphore(&mutex_write, "peek_tcp_write", 0, NU_PRIORITY);
	NU_Create_Semaphore(&mutex_close, "peek_tcp_close", 0, NU_PRIORITY);
	NU_Create_Semaphore(&mutex_dns, "peek_tcp_dns", 0, NU_PRIORITY);

	memset(s_tcp_sockets, 0, sizeof(s_tcp_sockets));
}

void peek_obtain_socket_semaphore() 
{
	emo_printf("peek_obtain_socket_semaphore() begin"); 
	NU_Obtain_Semaphore(&mutex_socket, NU_SUSPEND); 
	emo_printf("peek_obtain_socket_semaphore() end"); 
}

void peek_release_socket_semaphore() 
{ 
	emo_printf("peek_release_socket_semaphore() begin"); 
	NU_Release_Semaphore(&mutex_socket); 
	emo_printf("peek_release_socket_semaphore() end"); 
}

void peek_obtain_write_semaphore() 
{ 
	emo_printf("peek_obtain_write_semaphore() begin"); 
	NU_Obtain_Semaphore(&mutex_write, NU_SUSPEND); 
	emo_printf("peek_obtain_write_semaphore() end"); 
}

void peek_release_write_semaphore() 
{ 
	emo_printf("peek_release_write_semaphore() begin"); 
	NU_Release_Semaphore(&mutex_write); 
	emo_printf("peek_release_write_semaphore() end"); 
}

void peek_obtain_close_semaphore()
{
    emo_printf("peek_obtain_close_semaphore() begin");
    NU_Obtain_Semaphore(&mutex_close, NU_SUSPEND);
    emo_printf("peek_obtain_close_semaphore() end");
}

void peek_release_close_semaphore()
{
    emo_printf("peek_release_close_semaphore() begin");
    NU_Release_Semaphore(&mutex_close);
    emo_printf("peek_release_close_semaphore() end");
}

void peek_obtain_dns_semaphore()
{
    emo_printf("peek_obtain_dns_semaphore() begin");
    NU_Obtain_Semaphore(&mutex_dns, NU_SUSPEND);
    emo_printf("peek_obtain_dns_semaphore() end");
}

void peek_release_dns_semaphore()
{
    emo_printf("peek_release_dns_semaphore() begin");
    NU_Release_Semaphore(&mutex_dns);
    emo_printf("peek_release_dns_semaphore() end");
}

void peek_tcp_socket()
{
	int i;

	emo_printf("peek_socket()");

	for (i = 0; i < MAX_TCP_SOCKETS; i++)
	{
		if (!s_tcp_sockets[i].psocket)
		{
			PROC_CONTEXT_T *context = &s_tcp_sockets[i];
			memset(context, 0, sizeof(*context));

			context->waiting_for = WAITING_FOR_SOCKET;

			if (!app_socket(context, AP_TCPUL, SOCK_IPPROTO_TCP))
			{
				s_tcp_fd = -1;
				peek_release_socket_semaphore();
			}

			s_tcp_fd = i;
			return;
		}
	}

	if (i == MAX_TCP_SOCKETS)
	{
		s_tcp_fd = -1;
		peek_release_socket_semaphore();
	}
}

int peek_tcp_connect(int fd, const char *host, unsigned short port)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	emo_printf("peek_tcp_connect(fd=%d, host=%s, port=%d)", fd, host, port); 

	context->waiting_for = WAITING_FOR_CONNECT;

	app_connect(context, host, port);

	return 0;
}

void peek_tcp_write(int fd, const void *buf, int len)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	context->items_sent = len;

	emo_printf("peek_tcp_write(fd=%d, buf=%08X, len=%d)", fd, buf, len);

	context->waiting_for = WAITING_FOR_WRITE;

	app_send_buf(context, (char *)buf, len);
}

int peek_tcp_read(int fd, void *buf, int len)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];
	int nread = (len > context->data_rcvd ? context->data_rcvd : len);

	if (!nread)
	{
		errno = EAGAIN;
		return -1;
	}

	emo_printf("peek_tcp_read(fd=%d, buf=%08X, len=%d, nread=%d)", fd, buf, len, nread);

	memmove(buf, context->eventBuf, nread); 
//	memcpy(buf, context->eventBuf, nread); // FIXME this is not correct! should be memmove
	context->data_rcvd -= nread;
	return nread;
}

int peek_tcp_can_read(int fd)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	emo_printf("peek_tcp_can_read(fd=%d) = %d", fd, context->data_rcvd);

	if (context->waiting_for == RECEIVED_DISCONNECT && context->data_rcvd <= 0)
		return -1;

	return (context->data_rcvd > 0);
}

int peek_tcp_can_write(int fd)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	emo_printf("peek_tcp_can_write(fd=%d) = %d", fd, context->psocket);

	if (context->waiting_for == RECEIVED_DISCONNECT)
		return -1;

	return (context->pstate == PS_COMM);
}

void peek_tcp_enable_read(int fd)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	emo_printf("peek_tcp_enable_read(fd=%d)", fd);

	if (context->data_rcvd <= 0)
	{
		context->data_rcvd = 0;
		if (context->eventBuf)
		{
			p_free(context->eventBuf);
			context->eventBuf = 0;
		}
	}

	if (context->psocket)
	{
		//emo_printf("peek_tcp_enable_read() enable XON");
		//app_switch_flow(context, 1);
	}
}

void peek_tcp_dns_request(char *hostname)
{
	app_gethostbyname(hostname);
	p_free(hostname);
}

void peek_sleep(int ms)
{
	TCCE_Task_Sleep(ms);
}

void peek_proc_socket_event(PROC_CONTEXT_T *context, unsigned int event)
{
	static int init = 0;
	static NU_SEMAPHORE mutex_event;

	if (!init)
	{
		NU_Create_Semaphore(&mutex_event, "peek_proc_socket_event", 1, NU_PRIORITY);
		init = 1;
	}
	
    NU_Obtain_Semaphore(&mutex_event, NU_SUSPEND);

	emo_printf("peek_proc_socket_event() %d", event);
	switch (event)
	{
		case EMOBIIX_SOCK_CREA:
			context->waiting_for = WAITING_FOR_NONE;
			peek_release_socket_semaphore();
			break;

		case EMOBIIX_NETSURF_DNS:
			context->waiting_for = WAITING_FOR_NONE;
			s_tcp_dns_res = (unsigned long)context->server_ipaddr;
			peek_release_dns_semaphore();
			break;

		case EMOBIIX_SOCK_SENT:
			context->waiting_for = WAITING_FOR_NONE;
			s_tcp_send_res = context->items_sent;
			peek_release_write_semaphore();
			break;

		case EMOBIIX_SOCK_CONN:
			context->waiting_for = WAITING_FOR_NONE;
			break;

		case EMOBIIX_SOCK_DCON:
			context->psocket = 0;
			if (context->waiting_for == WAITING_FOR_CONNECT)
			{
			}
			else if (context->waiting_for == WAITING_FOR_WRITE)
			{
				s_tcp_send_res = -1;
				peek_release_write_semaphore();
			}
			else if (context->waiting_for == WAITING_FOR_DNS)
			{
				s_tcp_dns_res = 0;
				peek_release_dns_semaphore();
			}
			else if (context->waiting_for == WAITING_FOR_SOCKET)
			{
				s_tcp_fd = -1;
				peek_release_socket_semaphore();
			}

			context->waiting_for = RECEIVED_DISCONNECT;
			break;

		case EMOBIIX_SOCK_RECV:
			//if (context->psocket)
				app_switch_flow(context, 0);
			break;

		default:
			break;
	}

    NU_Release_Semaphore(&mutex_event);
}

