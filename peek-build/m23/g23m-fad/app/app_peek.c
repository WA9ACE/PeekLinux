#include "nucleus.h"
#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "socket_api.h"
#include "p_sim.h"
#include "app.h"
#include "p_malloc.h"

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

#define MAX_TCP_SOCKETS 	30
static PROC_CONTEXT_T s_tcp_sockets[MAX_TCP_SOCKETS];

static NU_SEMAPHORE mutex_socket;
//static NU_SEMAPHORE mutex_connect;
static NU_SEMAPHORE mutex_write;
static NU_SEMAPHORE mutex_close;
static NU_SEMAPHORE mutex_dns;

typedef struct _ReadData
{
	int nbytes;
	char *bytes;
} ReadData;

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
		if (!s_tcp_sockets[i].psocket && !(s_tcp_sockets[i].recv_list && list_size(s_tcp_sockets[i].recv_list) > 0))
		{
			PROC_CONTEXT_T *context = &s_tcp_sockets[i];

			if (context->recv_list)
				list_delete(context->recv_list);

			memset(context, 0, sizeof(*context));

			context->waiting_for = WAITING_FOR_SOCKET;
			context->recv_list = list_new();

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
	int nitems = list_size(context->recv_list);
	int nread;
	ListIterator it;
	ReadData *data;
	
	if (!nitems)
	{
		if (context->waiting_for == RECEIVED_DISCONNECT)
			return 0; // return EOF if disconnected and no data left

		errno = EAGAIN;
		return -1;
	}

	list_begin(context->recv_list, &it);
	data = (ReadData *)listIterator_item(&it);

	nread = (len > data->nbytes ? data->nbytes : len);

	emo_printf("peek_tcp_read(fd=%d, buf=%08X, len=%d, nread=%d, items=%d)", fd, buf, len, nread, nitems);

	memmove(buf, data->bytes, nread); 
	data->nbytes -= nread;

	if (data->nbytes <= 0)
	{
		p_free(data->bytes);
		p_free(data);
		listIterator_remove(&it);
	}
		
	return nread;
}

int peek_tcp_can_read(int fd)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];
	int nread = list_size(context->recv_list);

	emo_printf("peek_tcp_can_read(fd=%d) = %d", fd, nread);

	if (context->waiting_for == RECEIVED_DISCONNECT && nread <= 0)
		return 1; // dont fail a disconnected socket, let it return 0 from read instead -1;

	return nread > 0;
}

int peek_tcp_can_write(int fd)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	emo_printf("peek_tcp_can_write(fd=%d) = %d", fd, context->psocket);

	if (context->waiting_for == RECEIVED_DISCONNECT)
		return -1;

	return (context->pstate == PS_COMM);
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

const char *emo_event_to_str(int event)
{
#define EVENT_2_CASE_STR(a) case a: return #a
	switch (event)
	{
		EVENT_2_CASE_STR(EMOBIIX_SOCK_CREA);
		EVENT_2_CASE_STR(EMOBIIX_SOCK_CONN);
		EVENT_2_CASE_STR(EMOBIIX_SOCK_RECV);
		EVENT_2_CASE_STR(EMOBIIX_SOCK_DCON);
		EVENT_2_CASE_STR(EMOBIIX_WRITEMSG);
		EVENT_2_CASE_STR(EMOBIIX_NETSURF_SOCKET);
		EVENT_2_CASE_STR(EMOBIIX_NETSURF_CONNECT);
		EVENT_2_CASE_STR(EMOBIIX_NETSURF_SEND);
		EVENT_2_CASE_STR(EMOBIIX_NETSURF_START);
		EVENT_2_CASE_STR(EMOBIIX_NETSURF_DNS);
		default: return "??? EVENT";
	}
#undef EVENT_2_CASE_STR
}

void peek_proc_socket_event(PROC_CONTEXT_T *context, unsigned int event)
{
	emo_printf("peek_proc_socket_event() %s", emo_event_to_str(event));
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
		{
			ReadData *data = (ReadData *)p_malloc(sizeof(ReadData));
			data->nbytes = context->data_rcvd;
			data->bytes = context->eventBuf;

			context->eventBuf = NULL;
			context->data_rcvd = 0;

			list_append(context->recv_list, (void *)data);

			app_switch_flow(context, 1);
		}
		break;

		default:
			break;
	}
}

