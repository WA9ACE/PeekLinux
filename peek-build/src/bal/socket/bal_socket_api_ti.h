#ifndef __BAL_SOCKET_API_TI_H_20080227__
#define __BAL_SOCKET_API_TI_H_20080227__

#ifdef __cplusplus
extern "C" {
#endif

#include "socket_api.h"

/* Socket adapter layer struct */
struct timeval 
{
	long    tv_sec;
	long    tv_usec;
};
struct  hostent 
{
	char	*h_name;
	char	**h_aliases;
	short	h_addrtype;
	short	h_length;
	char	**h_addr_list;
#define h_addr h_addr_list[0]
};


struct sockaddr 
{
	unsigned short sa_family;
	char	sa_data[14];
};

struct in_addr 
{
	union 
    {
		struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { unsigned short s_w1,s_w2; } S_un_w;
		unsigned long S_addr;
	} S_un;
#define s_addr  S_un.S_addr
#define s_host  S_un.S_un_b.s_b2
#define s_net   S_un.S_un_b.s_b1
#define s_imp   S_un.S_un_w.s_w2
#define s_impno S_un.S_un_b.s_b4
#define s_lh    S_un.S_un_b.s_b3
};

struct sockaddr_in 
{
	short	sin_family;
	unsigned short	sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};

#define bal_htonl(a)          SOCK_HTONL(a)
#define bal_htons(a)          SOCK_HTONS(a)

// Callback function and ind type for network notify
typedef enum
{                                                            
	BAL_SOCKET_NETWORK_STATUS_IND	= 0xFF,
	BAL_SOCK_RESUME_IND				= 0xFE,
	BAL_SOCK_TIMEOUT_IND            = 0xFD,
	BAL_SOCK_NEED_HARD_RESET_IND	= 0xFC,
#ifdef BAL_IP_PUSH
	BAL_SOCK_IP_ADDR_IND				= 0xFB,
	BAL_SOCK_UDP_DATA_IND			= 0xFA,	
#endif
	BAl_SOCKET_NETWORK_LOST_IND		= SOCK_RESULT_NETWORK_LOST, 
	BAL_SOCK_CONN_CLOSED_IND		= SOCK_CONN_CLOSED_IND,
	BAL_SOCK_ERROR_IND				= SOCK_ERROR_IND,
	BAL_SOCK_BAERER_CLOSED_IND		= SOCK_BAERER_CLOSED_IND
}BAL_SOCKET_IND;
#ifdef BAL_IP_PUSH
typedef void (*BAL_NETWORK_CB)(BAL_SOCKET_IND, void*);
#else
typedef void (*BAL_NETWORK_CB)(BAL_SOCKET_IND);
#endif	//BAL_IP_PUSH

/* Socket adapter layer function */
int bal_sock_api_initialize( int app_handle,char* app_name);
void bal_sock_api_deinitialize();

int bal_socket (int __family, int __type, int __protocol);  
int bal_shutdown (int, int);
int bal_closesocket( int );
int bal_bind (int, const struct sockaddr_in *__my_addr, int __addrlen);
int bal_connect (int, const struct sockaddr_in *, int);
int bal_send (int, const void *__buff, int __len, unsigned int __flags);
int bal_recv (int, void *__buff, int __len, unsigned int __flags);

T_SOCK_IPADDR bal_gethostbyname(const char*);
unsigned long bal_inet_addr(const char*);

void bal_set_profile(const char* apn, const char* user, const char* pass);
int bal_get_socket_last_error(void);
void bal_socket_set_nm_status(unsigned char rl, unsigned char gs); 
void bal_socket_flight_mode();

#ifdef BAL_SELECT_PLMN
int get_bal_open_bearer_running();
#endif //BAL_SELECT_PLMN

#ifdef __cplusplus
}
#endif
#endif //__BAL_SOCKET_API_TI_H_20080227__

