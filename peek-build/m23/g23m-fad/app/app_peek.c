#include "nucleus.h"
#include "typedefs.h"
#include "vsi.h"
#include "pei.h"
#include "socket_api.h"
#include "p_sim.h"
#include "app.h"

extern void app_set_profile(char *apn, char *userid, char *password);
extern void app_connect_info(const char *server, unsigned short port);
extern int app_socket(PROC_CONTEXT_T *pcont, int __type, int __protocol);
extern BOOL app_send_buf(PROC_CONTEXT_T *pcont, char *buffer, int size);
extern void app_switch_flow(PROC_CONTEXT_T *pcont, int flow_on);
extern void app_close_tcp_bearer(PROC_CONTEXT_T *context);
extern void app_gethostbyname(const char *hostname);
void peek_tcp_enable_read(int fd);

#define MAX_TCP_SOCKETS 	5
static PROC_CONTEXT_T s_tcp_sockets[MAX_TCP_SOCKETS];

static NU_SEMAPHORE mutex_socket;
static NU_SEMAPHORE mutex_connect;
static NU_SEMAPHORE mutex_write;
static NU_SEMAPHORE mutex_close;
static NU_SEMAPHORE mutex_dns;

static int s_tcp_fd = -1;
int peek_tcp_get_socket_result()
{
	return s_tcp_fd;
}

static int s_tcp_connect_res = -1;
int peek_tcp_get_connect_result()
{
	return s_tcp_connect_res;
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
	app_set_profile("track.t-mobile.com", "getpeek", "txtbl123");
	//app_set_profile("wap.cingular", "WAP@CINGULARGPRS.COM", "CINGULAR1");

	NU_Create_Semaphore(&mutex_socket, "peek_tcp_socket", 0, NU_PRIORITY);
	NU_Create_Semaphore(&mutex_connect, "peek_tcp_connect", 0, NU_PRIORITY);
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
	NU_Reset_Semaphore(&mutex_socket, 0); 
	emo_printf("peek_release_socket_semaphore() end"); 
}

void peek_obtain_connect_semaphore() 
{ 
	emo_printf("peek_obtain_connect_semaphore() begin"); 
	NU_Obtain_Semaphore(&mutex_connect, NU_SUSPEND); 
	emo_printf("peek_obtain_connect_semaphore() end"); 
}

void peek_release_connect_semaphore() 
{ 
	emo_printf("peek_release_connect_semaphore() begin"); 
	NU_Reset_Semaphore(&mutex_connect, 0); 
	emo_printf("peek_release_connect_semaphore() end"); 
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
	NU_Reset_Semaphore(&mutex_write, 0); 
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
    NU_Reset_Semaphore(&mutex_close, 0);
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
    NU_Reset_Semaphore(&mutex_dns, 0);
    emo_printf("peek_release_dns_semaphore() end");
}

void peek_tcp_socket()
{
	PROC_CONTEXT_T *context;
	int i;

	emo_printf("peek_socket()");

	for (i = 0; i < MAX_TCP_SOCKETS; i++)
		if (!s_tcp_sockets[i].psocket)
			break;

	if (i == MAX_TCP_SOCKETS)
	{
		s_tcp_fd = -1;
	}
	else
	{
		context = &s_tcp_sockets[i];
		memset(context, 0, sizeof(*context));

		s_tcp_fd = i;
	}

	peek_release_socket_semaphore();
}

int peek_tcp_connect(int fd, const char *host, unsigned short port)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	emo_printf("peek_tcp_connect(fd=%d, host=%s, port=%d)", fd, host, port); 

	app_set_profile("track.t-mobile.com", "getpeek", "txtbl123");

	app_connect_info(host, port);
	//app_connect_info("10.150.9.6", 1234);
	app_socket(context, AP_TCPUL, SOCK_IPPROTO_TCP);

	context->waiting_for = WAITING_FOR_CONNECT;

	return 0;
}

void peek_tcp_write(int fd, const void *buf, int len)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	context->items_sent = len;

	emo_printf("peek_tcp_write(fd=%d, buf=%08X, len=%d)", fd, buf, len);

	app_send_buf(context, (char *)buf, len);

	context->waiting_for = WAITING_FOR_WRITE;
}

int peek_tcp_can_read(int fd)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

	emo_printf("peek_tcp_can_read(fd=%d) = %d", fd, context->data_rcvd);

	return context->data_rcvd > 0;
}

int peek_tcp_read(int fd, void *buf, int len)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];
	int nread = (len > context->data_rcvd ? context->data_rcvd : len);

	emo_printf("peek_tcp_read(fd=%d, buf=%08X, len=%d, nread=%d)", fd, buf, len, nread);

//	memmove(buf, context->eventBuf, nread); // FIXME this is not correct! should be memmove
	memcpy(buf, context->eventBuf, nread); // FIXME this is not correct! should be memmove
	context->data_rcvd -= nread;
	return nread;
}

void peek_tcp_enable_read(int fd)
{
	PROC_CONTEXT_T *context = &s_tcp_sockets[fd];

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
		app_switch_flow(context, 1);
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
	emo_printf("peek_proc_socket_event() %d", event);
	switch (event)
	{
		case EMOBIIX_NETSURF_DNS:
			s_tcp_dns_res = (unsigned long)context->server_ipaddr;
			peek_release_dns_semaphore();
			break;

		case EMOBIIX_SOCK_SENT:
			s_tcp_send_res = context->items_sent;
			peek_release_write_semaphore();
			break;

		case EMOBIIX_SOCK_CONN:
			s_tcp_connect_res = 0;
			peek_release_connect_semaphore();
			break;

		case EMOBIIX_SOCK_DCON:
			context->psocket = 0;
			if (context->waiting_for == WAITING_FOR_CONNECT)
			{
				s_tcp_connect_res = -1;
				peek_release_connect_semaphore();
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

			// XXX might need to semaphore this... 
	//		app_close_tcp_bearer(context);
	//		peek_sleep(10);
			//peek_obtain_close_semaphore();
			break;

		case EMOBIIX_SOCK_RECV:
			if (context->psocket)
				app_switch_flow(context, 0);
			break;

		default:
			break;
	}
}

