/*
 * QEMU driver for socket emulation
 * 
*/ 
#include "general.h"
#include "socket_api.h"
#include "Debug.h"
#include <stdint.h>

#define SOCK_BASE 0xFFFE9800
#define SOCK_FD     0x0
#define SOCK_WRET   0x4
#define SOCK_TYPE   0x4
#define SOCK_DOMAIN 0x8
#define SOCK_READADDR 0x8
#define SOCK_PROTO  0xC
#define SOCK_CREATE 0x10
#define SOCK_CONN   0x14
#define SOCK_CONTRANS 0x18
#define SOCK_SIZE   0x1c
#define SOCK_WTRANS 0x20
#define SOCK_WSIZE  0x24
#define SOCK_WTRG   0x28
#define SOCK_RTRG    0x2C
#define SOCK_RTRANS  0x30
#define SOCK_RSIZE   0x34
#define SOCK_RRET    0x10
#define SOCK_RBUF    0xc
#define SOCK_CRET    0x18

int bal_sock_api_inst;

typedef enum
{
        BAL_SOCKET_NETWORK_STATUS_IND   = 0xFF,
        BAL_SOCK_RESUME_IND                             = 0xFE,
        BAL_SOCK_TIMEOUT_IND            = 0xFD,
        BAL_SOCK_NEED_HARD_RESET_IND    = 0xFC,
#ifdef BAL_IP_PUSH
        BAL_SOCK_IP_ADDR_IND                            = 0xFB,
        BAL_SOCK_UDP_DATA_IND                   = 0xFA,
#endif
        BAl_SOCKET_NETWORK_LOST_IND             = SOCK_RESULT_NETWORK_LOST,
        BAL_SOCK_CONN_CLOSED_IND                = SOCK_CONN_CLOSED_IND,
        BAL_SOCK_ERROR_IND                              = SOCK_ERROR_IND,
        BAL_SOCK_BAERER_CLOSED_IND              = SOCK_BAERER_CLOSED_IND
}BAL_SOCKET_IND;

typedef void (*BAL_NETWORK_CB)(BAL_SOCKET_IND);

uint32_t SimReadReg(void) {
        void *regaddr;
        regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_READADDR);
	
	return *(unsigned int*) regaddr;

}
#ifdef EMO_SIM
int bal_socket (int __family, int __type, int __protocol) {

	void *regaddr;
 	emo_printf("bal_socket()\n");
	/* Trigger create */
	//regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_CREATE);
	 //*(unsigned int*) regaddr = 1;

	/* Get return value */
	//regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_FD);

	return 0;//*(unsigned int*) regaddr;
}

int bal_connect (int sockfd, const struct sockaddr_in *serv_addr, int addrlen) {

	void *regaddr;
	int i;

	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_SIZE);
	*(unsigned int*) regaddr = addrlen;
        regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_CONTRANS);
	
        emo_printf("bal_connect() addrlen %d\n", addrlen);

	for(i=0; i < addrlen;i++) {
//		emo_printf("copying: 0x%08X\n", *(uint32_t *)(((char *)serv_addr) + i));
		memcpy(((char *)regaddr), ((char *)serv_addr) +i, 1); 
	}
	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_CONN);
	*(unsigned int*) regaddr = 1;
	
	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_CRET);
	return *(unsigned int*) regaddr;
}

void bal_set_profile(const char* apn, const char* user, const char* pass) {
	emo_printf("bal_set_profile\n");
}

int bal_get_socket_last_error(void) {
	emo_printf("bal_get_socket_last_error\n");
	return 0;
}

void bal_socket_set_nm_status(unsigned char rl, unsigned char gs) {
	emo_printf("bal_socket_set_nm_status\n");
}
void bal_socket_flight_mode(void) {
	emo_printf("bal_socket_flight_mode\n");
}

int bal_inet_addr(const char *cp) 
{
    int a,b,c,d;
    char arr[4];

    emo_printf("bal_inet_addr()\n");

    sscanf(cp,"%d.%d.%d.%d",&a,&b,&c,&d);
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d;
    return *(unsigned int*)arr; 
}

void bal_set_network_cb(BAL_NETWORK_CB cb) {
    emo_printf("bal_set_network_cb()\n");
}

int bal_sock_api_initialize( int app_handle,char* app_name) {
    emo_printf("bal_sock_api_initialize\n");
    return 1;
}

void bal_sock_api_deinitialize(void) {
    emo_printf("bal_sock_api_deinitialize()\n");
}

void bal_socket_reset_aci_layle(void) {
    emo_printf("bal_socket_reset_aci_layle()\n");
}

int bal_shutdown (int sock, int how) {
    emo_printf("Bal_shutdown()\n");
    return 0;
}

int bal_bind(int sockfd, struct sockaddr_in *my_addr, int addrlen) {
    emo_printf("Bal_bind() \n");
    return 0;
}

int bal_send (int s, const void *__buff, int __len, unsigned int __flags) {

        void *regaddr;
        int i;

        regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_WSIZE);
        *(unsigned int*) regaddr = __len;
        regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_WTRANS);

        emo_printf("bal_send() len %d\n", __len);

        for(i=0; i < __len;i+=1) {
             //   emo_printf("send copying: 0x%08X\n", *(uint32_t *)(((char *)__buff) + i));
                memcpy(((char *)regaddr), ((char *)__buff) +i, 1);
        }
        regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_WTRG);
        *(unsigned int*) regaddr = 1;
	 NU_Sleep(100);
	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_WRET);

	return *(unsigned int*) regaddr; 
}

int bal_recv (int s, void *__buff, int __len, unsigned int __flags) {
        void *regaddr;
        int i;
	char tbuf;
	unsigned int x;

	emo_printf("Bal_recv()\n");

	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_RSIZE);
	*(unsigned int*) regaddr = __len; // Set Size of read
	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_RTRG);
	*(unsigned int*) regaddr = 1;// Trigger Read
	NU_Sleep(100);
	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_RRET);
	__len = *(unsigned int*) regaddr;

	if (__len < 0) {
		regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_RTRANS);
		*(unsigned int*) regaddr = 1;// Free Read buffer
		return -1;
	}

	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_RBUF);
	emo_printf("Bal_recv reading %d\n", __len);
        for(i=0; i < __len;i++) {
		tbuf = *(unsigned int*)regaddr;
                memcpy(((char *)__buff) +i, &tbuf, 1); // 1 byte chunks
//                emo_printf("recv copying: 0x%08X\n", tbuf);
        }

        regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_RTRANS);
        *(unsigned int*) regaddr = 1; // Free Read buffer
	NU_Sleep(100);
	regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_RRET);
	x = *(unsigned int*) regaddr;
	emo_printf("Bal_recv returning %d\n", x);
	return x;
}

int bal_closesocket(int fd) {
    void *regaddr;

    emo_printf("Bal_closesocket()\n");
    regaddr = (void*)((unsigned int)SOCK_BASE+SOCK_FD);
    *(unsigned int*) regaddr = 0;

    return 0;
}
#endif
