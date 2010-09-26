#ifndef __SYS_SOCK__
#define __SYS_SOCK__
#include "ngresolv.h"
#include "peekerrno.h"

#define htons(a) ((((a) & 0xff) << 8) | (((a) & 0xff00) >> 8))
#define ntohs(a) htons(a)



struct sockaddr {
	unsigned char sa_len; /* total length */
	unsigned char sa_family; /* address family */
	unsigned char sa_data[14]; /* address value */
};

struct in_addr {
	unsigned int s_addr; /* 32 bits IP address, net byte order */
};

#define FD_SETSIZE 32

typedef struct {
	unsigned long fds[(FD_SETSIZE+31)/32];
} fd_set;

#define FD_ZERO( fdset) { int i; for( i=0; i<((FD_SETSIZE+31)/32); i++) (fdset)->fds[i] = 0; }
#define FD_SET( fd, fdset) ((fdset)->fds[(fd)>>5] |= 1UL<<((fd) & 0x1f))
#define FD_CLR( fd, fdset)  ((fdset)->fds[(fd)>>5] &= ~(1UL<<((fd) & 0x1f)))
#define FD_ISSET( fd, fdset) (((fdset)->fds[(fd)>>5] & (1UL<<((fd) & 0x1f))) != 0UL)

struct sockaddr_in {
	unsigned char sin_len; /* total length (16) */
	unsigned char sin_family; /* AF_INET */
	unsigned short sin_port; /* 16 bits port number, net byte order */
	struct in_addr sin_addr; /* 32 bits IP address */
	unsigned char sin_zero[8]; /* unused */
};

struct hostent *gethostbyname(const char *name);

struct hostent
{
	char *h_name;                 /* Official name of host.  */
	char **h_aliases;             /* Alias list.  */
	int h_addrtype;               /* Host address type.  */
	int h_length;                 /* Length of address.  */
	char **h_addr_list;           /* List of addresses from name server.  */
#define h_addr  h_addr_list[0]  /* Address, for backward compatibility.  */
};

#define MSG_NOSIGNAL        0x4000  /* Do not generate SIGPIPE */
#endif
