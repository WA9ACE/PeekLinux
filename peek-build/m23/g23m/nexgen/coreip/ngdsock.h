/*****************************************************************************
 * $Id: ngdsock.h,v 1.5 2002/04/25 08:37:56 rf Exp $
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
 * 07/10/98 - Regis Feneon
 * 12/12/98 -
 *  Change socket recv/send buffers organization
 * 06/01/99 -
 *  changes in proprietary socket interface
 *  new NGsockctrl structure, ngSockOpen() function changed
 * 13/01/99 -
 *  added NGtimeval and NGlinger for socket options
 *  NG_SOCKBUF_XXX macros
 * 19/01/99 -
 *  ngSoSleepOn() added
 * 11/04/99 -
 *  SOL_IP, SOL_TCP, SOL_UDP added
 * 28/05/99 -
 *  added so_ipproto in NGsock structure
 *  added NG_SO_HDRINCL in so_options
 * 12/08/99 -
 *  ngSockFdGetPtr(), ngSockGetState(), ngSockReadByte() added
 * 10/09/99 -
 *  Flags NG_IOSTATE_ISDISCONNECTED & NG_IOSTATE_HASERROR added
 *  for ngSockGetState()
 * 11/10/99 -
 *  NG_IOSTATE_XXX & NGtimeval moved to ng_stdio.h
 *  now include ng_stdio.h
 *  ngSockGetState() prototype changed
 *  ngSockBufFree() prototype changed
 *  ngSockGetNRead() prototype added
 *  ngSoOption() prototype changed
 * 09/01/2000 -
 *  added prototypes for ngSockPrintf() and ngSockVPrintf()
 * 09/02/2000 _
 *  added NG_IF_MULTICAST options
 *  added so_mcast fields in NGsock structure
 * 11/02/2000 -
 *  new proto for ngSockVPrintf() and ngSockPrintf()
 * 24/02/2000 -
 *  added NG_IF_MULTICAST_IFP option
 * 08/06/2000 -
 *  changed ngSoFindEphemeral to return (void)
 * 19/07/2000 -
 *  NG_PRU_XXX and NG_PRC_XXX moved to netprot.h
 * 04/09/2000 -
 *  added prototypes for ngSockInChar(),ngSockOutChar(),ngSoRecv(),ngSoSend()
 * 14/09/2000 -
 *  added NG_SOCKBUF_SETIF() macro
 * 03/11/2000 -
 *  added IGMP socket options
 * 14/02/2001 -
 *  part of this file moved to ngsockio.h
 *  added ngSockCreate(), ngSockBind(), ngSockConnect(), ngSockListen(),
 *   ngSockSelect()
 *****************************************************************************/

#ifndef __NG_NGDSOCK_H_INCLUDED__
#define __NG_NGDSOCK_H_INCLUDED__

#include <ngip.h>
#include <ngsockio.h>

NG_BEGIN_DECLS

/*****************************************************************************
 * Public
 *****************************************************************************/

/* socket open control structure */
typedef struct {
    int sc_family; /* protocol family: PF_INET or AF_INET */
    int sc_type; /* type of socket (SOCK_DGRAM,SOCK_STREAM) */
    int sc_protocol; /* protocol for raw sockets */
    int sc_flags; /* open flags: */
#define NG_SOCKF_NONBLOCK 0x0001 /* non-blocking operations */
#define NG_SOCKF_PASSIVE 0x0002 /* force passive open */
    NGsockaddr sc_laddr; /* local address and port */
    NGsockaddr sc_faddr; /* foreign address and port */
    int sc_options; /* socket options */
    int sc_timeo; /* connection time-out (seconds) */
    int sc_backlog; /* maximum number of pending connections */
    int sc_linger; /* linger time (seconds) */
    int sc_rcv_buf; /* size of receive buffer */
    int sc_snd_buf; /* size of send buffer */
    int sc_rcv_lowat; /* low water mark */
    int sc_snd_lowat;
    u_long sc_rcv_timeo_ms; /* receive timeout (milliseconds) */
    u_long sc_snd_timeo_ms; /* send timeout (milliseconds) */
} NGsockctrl;

int ngSockAccept( NGsock *so, NGsockaddr *addr, NGsock **newso);
int ngSockBind( NGsock *so, NGsockaddr *addr);
int ngSockClose( NGsock *so);
int ngSockConnect( NGsock *so, NGsockaddr *addr);
int ngSockCreate( NGsock **so, int af, int type, int proto, int oflags);
int ngSockFdGetPtr( int fd, NGsock **so);
int ngSockGetOption( NGsock *so, int level, int option, void *optval, int *optlen);
int ngSockGetState( NGsock *so);
int ngSockGetNRead( NGsock *so);
int ngSockListen( NGsock *so, int backlog);
int ngSockOpen( NGsockctrl *ctrlp, NGsock **newso);
int ngSockRecv( NGsock *so, void *datap, int datalen, int *flags, NGsockaddr *addr);
int ngSockRecvv( NGsock *so, NGiovec *iov, int iovcnt, int *flags, NGsockaddr *addr);
int ngSockSelect( NGiosel *solst, int nso, NGtimeval *timeout);
int ngSockSend( NGsock *so, void *datap, int datalen, int flags, NGsockaddr *addr);
int ngSockSendv( NGsock *so, NGiovec *iov, int iovcnt, int flags, NGsockaddr *addr);
int ngSockSetCallback( NGsock *so, void (*upcall_f)( NGsock *, void *, int),
                       void *upcall_data);
int ngSockSetOption( NGsock *so, int level, int option, const void *optval, int optlen);
int ngSockShutdown( NGsock *so);
int ngSockReadByte( NGsock *so, int flags);
int ngSockPrintf( NGsock *so, char *fmt,...);
int ngSockVPrintf( NGsock *so, char *fmt, NGva_list args);
/* zero-copy interface */
int ngSockBufAlloc( NGsock *so, NGbuf **bufp);
int ngSockBufFree( NGsock *so, NGbuf *bufp);
int ngSockBufRecv( NGsock *so, int *flags, NGsockaddr *addr, NGbuf **bufp);
int ngSockBufSend( NGsock *so, NGbuf *bufp, int flags, NGsockaddr *addr);

/* standard input/output redirections */
int ngSockInChar( void *data);
void ngSockOutChar( int c, void *data);

#ifndef NG_NO_BSDSOCK_DECLS

/*
 * BSD compatible names
 * this should be in ngsocket.h
 */

/* address family */
#define AF_UNSPEC NG_AF_UNSPEC
#define AF_INET NG_AF_INET
/* protocol family (== address family) */
#define PF_UNSPEC NG_AF_UNSPEC
#define PF_INET NG_AF_INET
/* types of socket */
#define SOCK_STREAM NG_SOCK_STREAM
#define SOCK_DGRAM NG_SOCK_DGRAM
#define SOCK_RAW NG_SOCK_RAW
/* level of option */
#define SOL_SOCKET NG_IOCTL_SOCKET
#define SOL_IP NG_IOCTL_IP
#define SOL_TCP NG_IOCTL_TCP
#define SOL_UDP NG_IOCTL_UDP
/* socket level options */
#define SO_DEBUG NG_SO_DEBUG
#define SO_ACCEPTCONN NG_SO_ACCEPTCONN
#define SO_REUSEADDR NG_SO_REUSEADDR
#define SO_REUSEPORT NG_SO_REUSEPORT
#define SO_KEEPALIVE NG_SO_KEEPALIVE
#define SO_DONTROUTE NG_SO_DONTROUTE
#define SO_BROADCAST NG_SO_BROADCAST
#define SO_LINGER NG_SO_LINGER
#define SO_OOBINLINE NG_SO_OOBINLINE
#define SO_SNDBUF NG_SO_SNDBUF
#define SO_RCVBUF NG_SO_RCVBUF
#define SO_SNDLOWAT NG_SO_SNDLOWAT
#define SO_RCVLOWAT NG_SO_RCVLOWAT
#define SO_SNDTIMEO NG_SO_SNDTIMEO
#define SO_RCVTIMEO NG_SO_RCVTIMEO
#define SO_ERROR NG_SO_ERROR
#define SO_TYPE NG_SO_TYPE
/* ip level options */
#define IP_HDRINCL NG_IP_HDRINCL
#define IP_TOS NG_IP_TOS
#define IP_TTL NG_IP_TTL
#define IP_MULTICAST_IF NG_IP_MULTICAST_IF
#define IP_MULTICAST_TTL NG_IP_MULTICAST_TTL
#define IP_MULTICAST_LOOP NG_IP_MULTICAST_LOOP
#define IP_ADD_MEMBERSHIP NG_IP_ADD_MEMBERSHIP
#define IP_DROP_MEMBERSHIP NG_IP_DROP_MEMBERSHIP
#define IP_RECVDSTADDR NG_IP_RECVDSTADDR
/* recv/send flags */
#define MSG_DONTWAIT NG_IO_DONTWAIT
#define MSG_WAITALL NG_IO_WAITALL
#define MSG_OOB NG_IO_OOB
#define MSG_PEEK NG_IO_PEEK
#define MSG_DONTROUTE NG_IO_DONTROUTE
#define MSG_EOR NG_IO_EOR
#define MSG_TRUNC NG_IO_TRUNC
#define MSG_CTRUNC NG_IO_CTRUNC
#define MSG_BCAST NG_IO_BCAST
#define MSG_MCAST NG_IO_MCAST

#endif /* NG_NO_BSDSOCK_DECLS */

NG_END_DECLS

#endif

