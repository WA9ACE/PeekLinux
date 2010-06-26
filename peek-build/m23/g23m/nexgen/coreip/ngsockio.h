/*****************************************************************************
 * $Id: ngsockio.h,v 1.6 2001/03/21 10:05:38 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Socket Interface Definitions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1999-2001 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  part of this file from ngdsock.h
 *  added asynchronous socket interface
 *  added flags to ngSoSleepOn() and ngSoWakeup() prototype
 * 18/02/2001 -
 *  added NGsockbufaddr & NG_IP_RECVDSTADDR
 *****************************************************************************/

#ifndef __NG_NGSOCKIO_H_INCLUDED__
#define __NG_NGSOCKIO_H_INCLUDED__

#include <ngip.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/*****************************************************************************
 * Public
 *****************************************************************************/

/* address family */
#define NG_AF_UNSPEC 0 /* unspecified */
#define NG_AF_INET 2 /* internetwork: UDP, TCP, etc. */

/* type of socket */
#define NG_SOCK_STREAM 1
#define NG_SOCK_DGRAM 2
#define NG_SOCK_RAW 3

/* level of option */
#define NG_IOCTL_SOCKET 0xffffU /* options for socket level */
#define NG_IOCTL_IP IPPROTO_IP /* IP level (raw sockets) */
#define NG_IOCTL_TCP IPPROTO_TCP /* TCP level */
#define NG_IOCTL_UDP IPPROTO_UDP /* UDP level */
/* set/get operation */
#define NG_SO_SET 0x8000U /* set option */
#define NG_SO_GET 0x0000U /* get option */
/* socket level options */
#define NG_SO_SNDBUF 0x1001 /* send buffer size */
#define NG_SO_RCVBUF 0x1002 /* receive buffer size */
#define NG_SO_SNDLOWAT 0x1003 /* send low-water mark */
#define NG_SO_RCVLOWAT 0x1004 /* receive low-water mark */
#define NG_SO_SNDTIMEO 0x1005 /* send timeout */
#define NG_SO_RCVTIMEO 0x1006 /* receive timeout */
#define NG_SO_ERROR 0x1007 /* get error status and clear (ro) */
#define NG_SO_TYPE 0x1008 /* get socket type (ro) */
/* NGDSock specific options */
#define NG_SO_NBIO 0x1f01 /* set nonblocking mode (NG_SS_NBIO flag) */
#define NG_SO_NREAD 0x1f02 /* nb of bytes in rcv buffer (ro) */
#define NG_SO_ATMARK 0x1f03 /* returns SS_RCVATMARK flag (ro) */
#define NG_SO_SOCKNAME 0x1f04 /* get socket name (ro) */
#define NG_SO_PEERNAME 0x1f05 /* get peer name (ro) */
/* IP level options */
#define NG_IP_HDRINCL 0x0002 /* header is included with data (raw) */
#define NG_IP_TOS 0x0003 /* IP type of service and precedence */
#define NG_IP_TTL 0x0004 /* IP time to live */
#define NG_IP_MULTICAST_IF 0x0005 /* Interface for outgoing multicasts */
#define NG_IP_MULTICAST_TTL 0x0006 /* time to live for outgoing multicasts */
#define NG_IP_MULTICAST_IFP 0x0007 /* interface for outgoing multicasts */
#define NG_IP_ADD_MEMBERSHIP 0x0008 /* add a mcast group membership */
#define NG_IP_ADD_MEMBERSHIP_IFP 0x0009
#define NG_IP_DROP_MEMBERSHIP 0x000a /* drop a mcast group membership */
#define NG_IP_DROP_MEMBERSHIP_IFP 0x000b
#define NG_IP_MULTICAST_LOOP 0x000c /* enable loopback of outgoing mcasts */
#define NG_IP_DONTFRAG 0x000d /* set DF bit on outgoing datagrams */
#define NG_IP_RECVDSTADDR 0x000e /* recv dest address in control data */
/* flags for recv/send operations */
#define NG_IO_DONTROUTE 0x0010 /* bypass routing table */
#define NG_IO_EOR 0x0020 /* end of record */
#define NG_IO_TRUNC 0x0040 /* rcvd data truncated */
#define NG_IO_CTRUNC 0x0080 /* rcvd control data truncated */
#define NG_IO_BCAST 0x0100 /* rcvd link-layer bcast */
#define NG_IO_MCAST 0x0200 /* rcvd link-layer mcast */

/* socket address structure */
typedef struct {
    NGubyte sin_len; /* address length (not used) */
    NGubyte sin_family; /* address family (not used) */
    NGushort sin_port; /* port number (net byte order) */
    NGuint sin_addr; /* ip address (net byte order) */
} NGsockaddr;

/* used by NG_SO_LINGER */
typedef struct {
    int l_onoff; /* option on/off */
    int l_linger; /* linget time in seconds */
} NGlinger;

/* used by NG_IP_ADD/DROP_MEMBERSHIP */
typedef struct {
  NGuint imr_multiaddr; /* group multicast address */
  NGuint imr_interface; /* IP address of interface */
  NGifnet *imr_ifp; /* pointer to interface */
} NGip_mreq;

/* used by select */
typedef struct {
  NGsock *sock; /* socket control block */
  int events; /* requested events */
  int revents; /* returned events */
#define NG_SIOSEL_READ 0x0001
#define NG_SIOSEL_WRITE 0x0002
#define NG_SIOSEL_EXCEPT 0x0004
#define NG_SIOSEL_ERROR 0x0008
  int fd; /* for selectsocket() */
} NGiosel;

/* max iovec structures for ngSAIOSendv() and ngSAIORecvv() */
#define NG_SOCK_IOVMAX 8

/*
 * Asynchronous Socket Interface
 */

int ngSAIOAccept( NGsock *so, NGsockaddr *addr, int flags, NGsock **newso);
int ngSAIOBind( NGsock *so, NGsockaddr *addr);
int ngSAIOBufAlloc( NGsock *so, NGbuf **bufp);
int ngSAIOBufFree( NGsock *so, NGbuf *bufp);
int ngSAIOBufRecv( NGsock *so, int *flagsp, NGsockaddr *from, NGbuf **rbufp,
                   NGuint *dstaddr);
int ngSAIOBufSend( NGsock *so, NGbuf *bufp, int flags, NGsockaddr *to);
int ngSAIOClose( NGsock *so, int flags);
int ngSAIOConnect( NGsock *so, NGsockaddr *addr, int flags);
int ngSAIOCreate( NGsock **so, int af, int type, int proto, int oflags);
/* int ngSAIOGetOption( NGsock *so, int level, int option, void *optval,
                        int *optlen); */
int ngSAIOListen( NGsock *so, int backlog);
int ngSAIOOption( NGsock *so, int level, int option, void *optval,
                   int *optlen);
int ngSAIORecv( NGsock *so, void *buf, int buflen, int *flags,
                NGsockaddr *from, NGuint *dstaddr);
int ngSAIORecvv( NGsock *so, NGiovec *iov, int iovcnt, int *flagsp,
                 NGsockaddr *from, NGuint *dstaddr);
int ngSAIOSelect( NGiosel *solst, int nso, NGtimeval *timeout);
int ngSAIOSend( NGsock *so, void *buf, int buflen, int flags,
                NGsockaddr *to);
int ngSAIOSendv( NGsock *so, NGiovec *iov, int iovcnt, int flags,
                 NGsockaddr *to);
int ngSAIOSetCallback( NGsock *so, void (*upcall_f)( NGsock *, void *, int),
                       void *upcall_data);
int ngSAIOSetOption( NGsock *so, int level, int option, void *optval,
                     int optlen);
int ngSAIOShutdown( NGsock *so, int how);
#define ngSAIOGetOption( so, lvl, opt, optval, optlen) \
  ngSAIOOption( so, lvl, NG_SO_GET|(opt), optval, optlen)

/* upcall routine flags */
#define NG_SAIO_READ NG_SIOSEL_READ /* data received */
#define NG_SAIO_WRITE NG_SIOSEL_WRITE /* data sent */
#define NG_SAIO_EXCEPT NG_SIOSEL_EXCEPT /* OOB data received */
#define NG_SAIO_ERROR NG_SIOSEL_ERROR /* socket error */

/* zero-copy buffer macros */
#define NG_SOCKBUF_PTR( bufp) ((void *)((bufp)->buf_datap))
#define NG_SOCKBUF_LEN( bufp) ((bufp)->buf_datalen)
#define NG_SOCKBUF_SETLEN( bufp, len) { \
    (bufp)->buf_datalen = (len); \
}
#define NG_SOCKBUF_SETIF( bufp, netp) { \
    (bufp)->buf_flags |= NG_BUFF_SETIF; \
    (bufp)->buf_ifnetp = (netp); \
}

/*****************************************************************************
 * Private
 *****************************************************************************/

/* address stored in buffer (udp) */
typedef struct {
  NGsockaddr sba_src; /* source addr & port */
  NGuint sba_dst; /* dest addr */
} NGsockbufaddr;

/* default socket buffers 'lowat' */
#define NG_SOCK_RECV_LOWAT 1
#define NG_SOCK_SEND_LOWAT 1024

/* internal prototypes */
NGsock *ngSoAlloc( NGsock *head);
void ngSoFree( NGsock *so);
int ngSoBind( NGsock *so, NGsockaddr *addr, NGsock *head,
                   NGushort *eport);
int ngSoConnect( NGsock *so, NGsockaddr *addr, NGsock *head,
                       NGushort *eport);
NGsock *ngSoLookup( NGsock *head, NGuint laddr, NGushort lport, NGuint faddr,
                     NGuint fport, int wildok);
void ngSoIsConnected( NGsock *so);
void ngSoIsDisconnected( NGsock *so);
#ifdef NG_RTOS
int ngSoSleepOn( NGsock *so, int flags, u_long timeo);
#define ngSoSleepOnRecv( so, timeo) \
  ngSoSleepOn( so, NG_SIOWAIT_READ, timeo)
#define ngSoSleepOnSend( so, timeo) \
  ngSoSleepOn( so, NG_SIOWAIT_WRITE, timeo)
#define ngSoSleepOnCnct( so, timeo) \
  ngSoSleepOn( so, NG_SIOWAIT_READ|NG_SIOWAIT_WRITE, timeo)
#endif /* NG_RTOS */
void ngSoWakeUp( NGsock *so, int flags);
int ngSoGetEvents( NGsock *so, int flags);
void ngSoFindEphemeral( NGsock *so, NGsock *head, NGushort *eport);

/* call socket user function */
#define NG_SOCKCALL_USER( so, code, data, len, addr) \
 ( (int (*)( NGsock *, int, void *, int, NGsockaddr *)) \
 (((NGproto *) (so)->so_proto)->pr_user_f))( so, code, data, len, addr)

/* free receive socket buffer */
#define NG_SOCK_RCVBUF_FLUSH( so) { \
  NGbuf *bufp, *tbufp; \
  while( 1) { \
    NG_QUEUE_OUT( &((so)->so_rcv_q), bufp); \
    if( bufp == NULL) break; \
    while( bufp) { \
      tbufp = bufp->buf_next; \
      ngBufFree( bufp); \
      bufp = tbufp; \
    } \
  } \
  (so)->so_rcv_cc = 0; \
}

/* socket state changes macros */
#define ngSoIsConnecting( so) { \
  (so)->so_state |= NG_SS_ISCONNECTING; \
}

#define ngSoDataReceived( so) \
  ngSoWakeUp( (so), NG_SIOSEL_READ|NG_SIOWAIT_READ)

#define ngSoDataSent( so) \
  ngSoWakeUp( (so), NG_SIOSEL_WRITE|NG_SIOWAIT_WRITE)

#define ngSoIsDisconnecting( so) { \
  (so)->so_state &= ~NG_SS_ISCONNECTING; \
  (so)->so_state |= NG_SS_ISDISCONNECTING|NG_SS_CANTRCVMORE|NG_SS_CANTSENDMORE; \
  ngSoWakeUp( (so), NG_SIOSEL_READ|NG_SIOSEL_WRITE|NG_SIOWAIT_READ|NG_SIOWAIT_WRITE); \
}

#define ngSoCantRcvMore( so) { \
  (so)->so_state |= NG_SS_CANTRCVMORE; \
  ngSoWakeUp( (so), NG_SIOSEL_READ|NG_SIOWAIT_READ); \
}

#define ngSoCantSendMore( so) { \
  (so)->so_state |= NG_SS_CANTSENDMORE; \
  ngSoWakeUp( (so), NG_SIOSEL_WRITE|NG_SIOWAIT_WRITE); \
}

/* NG_END_DECLS // confuses Source Insight */

#endif

