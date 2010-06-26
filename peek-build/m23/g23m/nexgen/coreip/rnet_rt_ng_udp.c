/*****************************************************************************
 * $Id: udp.c,v 1.6.2.1 2002/10/28 18:49:49 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * User Datagram Protocol
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2002 NexGen Software.
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
 * udpInit()
 * udpInput()
 * udpUser()
 * ngProto_UDP
 *----------------------------------------------------------------------------
 * 04/11/98 - Regis Feneon
 * 03/12/98 -
 *  Applyed changes to protocol interface
 *  support for "iovec" data
 * 10/12/98 -
 *  ip_len adjustement in udpInput()
 * 11/12/98 -
 *  Use global synchro NG_SOCK_LOCK/NG_SOCK_UNLOCK
 * 12/12/98 -
 *  Changes in send/recv buffers
 *  Added protocol flag NG_PR_ATOMIC used by socket send/recv functions
 *  NG_SS_RCVARMED flag setting removed in NG_PRU_RCVD
 *  (moved to ngSockRecvv() function)
 * 08/01/99 -
 *  added direct interface
 *  added zero-copy send function (PRU_SENDBUF/PRU_SENDBUFINI)
 *  now use ngSoDataReceived() macro
 * 05/02/99 -
 *  full fragmentation support added
 * 29/05/99 -
 *  change NG_UDP_FRAGMAX for NG_SOCK_IOVMAX
 * 09/02/2000 -
 *  added support for multicast in NG_PRU_SEND
 * 18/02/2000 - Adrien Felon
 *  udpUser(): adding argument "flag" for ngIpOutput() call
 * 07/04/2000 -
 *  free buffer to send via zero copy interface if any error in udpUser()
 * 09/06/2000 - Regis Feneon
 *  cleanup initialization of static data
 * 14/09/2000 -
 *  added test for NG_BUFF_SETIF flag in NG_PRU_SENDBUF command
 * 19/09/2000 -
 *  udpInit() - reset ephemeral port and direct interface
 * 16/11/2000 -
 *  added UDP statistics
 * 24/11/2000 -
 *  new ngProtoList & NGproto structure
 *  udpInput() - added reception of multicast datagrams
 *               new argument to ngSoLookup()
 *  udpUser()/NG_PRU_SEND - support of DONTFRAG option
 * 23/01/2001 -
 *  corrections in NGiphdr structure
 * 14/02/2001 -
 *  allow re-connection and disconnection of socket
 *  removed NG_PRU_DISCONNECT
 * 18/02/2001 -
 *  udpInput() - store dest address in message
 *  udpUser()/PRU_SEND - pass DONTROUTE & MCASTLOOP flags to ngIpOutput()
 * 26/03/2001 -
 *  disconnect set bad so_state flags
 * 28/05/2001 -
 *  global data moved to udp_data.c
 *  uses ngIpTmpIov[]
 * 28/10/2002 -
 *  removed some warnings with msvc
 *****************************************************************************/

#include <ngip.h>
#include <ngudp.h>
#include <ngip/ip.h>
#include <ngip/udp.h>
#include <ngdsock.h>


static NGushort udpPort; /* ephemeral port */
/* direct interface */
NGushort ngUdp_DirectPort;
void (*ngUdp_DirectInput_f)( NGbuf *bufp, NGsockaddr *from);

/*****************************************************************************
 * udpInit()
 *****************************************************************************
 * Protocol Initialization
 *****************************************************************************
 */

static void udpInit( void)
{
  /* initialize socket list */
  ngUdp_Sockq.next = ngUdp_Sockq.prev = &ngUdp_Sockq;

  /* initialize ephemeral port */
  udpPort = IPPORT_RESERVED;

  /* direct interface */
  ngUdp_DirectPort = 0;
  ngUdp_DirectInput_f = NULL;

#ifdef NG_IPSTATS_SUPPORTED
  ngMemSet( &ngUdpStat, 0, sizeof( ngUdpStat));
#endif

}

/*****************************************************************************
 * udpInput()
 *****************************************************************************
 * Process input messages
 *****************************************************************************
 */
 

static void udpInput( NGbuf *bufp)
{
  NGiphdr *ip;
  NGudphdr *uh;
  NGsock *so;
/*  NGsockaddr addr; */
  int hlen, len, direct;
#ifdef NG_IPFRAG_SUPPORTED
  int f_iovcnt;
  NGbuf *f_bufp;
#endif
  NGuint ip_dst, ip_src;
  NGushort uh_dport, uh_sport;
  so = NULL;

  /* pointer to IP header */
  ip = (NGiphdr *) bufp->buf_datap;
  hlen = (ip->ip_hlv & NG_IPHDRLEN_MASK)<<2;
  ip->ip_len -= hlen; /* total ip data length */
  uh = (NGudphdr *) (((NGubyte *) ip) + hlen);
  /* buf_datap now points to udp data */
  bufp->buf_datap += hlen + sizeof( NGudphdr);
  bufp->buf_datalen -= hlen + sizeof( NGudphdr);
#ifdef NG_DEBUG
  ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 0, "%I:%d -> %I:%d", ip->ip_src,
          ngNTOHS( uh->uh_sport), ip->ip_dst, ngNTOHS( uh->uh_dport));
#endif

  /*
   * test message validity
   */
  len = ngNTOHS( uh->uh_ulen);
  /* ip and udp length should match... */
  if( len > ip->ip_len) {
#ifdef NG_DEBUG
    ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 1, "Invalid size");
#endif
#ifdef NG_IPSTATS_SUPPORTED
    ngUdpStat.udps_ierrors++;
#endif
    goto drop;
  }
  if( len < ip->ip_len) {
    /* IP len > UDP len: we must remove the extra bytes */
#ifdef NG_IPFRAG_SUPPORTED
    if( bufp->buf_flags & NG_BUFF_MOREDATA) {
      /* ... */
    }
    else {
      bufp->buf_datalen -= ip->ip_len - len;
    }
#else
    bufp->buf_datalen -= ip->ip_len - len;
#endif
  }

  /* checksum */
  if( uh->uh_sum) {
    if( (unsigned)hlen > sizeof( NGiphdr)) {
      /* there are ip options... strip them */
      ngMemMove( ((NGubyte *) ip)+(hlen - sizeof( NGiphdr)),
                 ip, sizeof( NGiphdr));
      ip = (NGiphdr *) (((NGubyte *) ip) + (hlen - sizeof( NGiphdr)));
    }
    /* create pseudo udp/ip header */
    ((NGudpiphdr *) ip)->ui_zero1 = 0;
    ((NGudpiphdr *) ip)->ui_zero2 = 0;
    ((NGudpiphdr *) ip)->ui_zero3 = 0;
    ((NGudpiphdr *) ip)->ui_len = uh->uh_ulen;
#ifdef NG_IPFRAG_SUPPORTED
    if( bufp->buf_flags & NG_BUFF_MOREDATA) {
      f_iovcnt = 0;
      f_bufp = bufp->buf_next;
      while( f_bufp) {
        if( f_iovcnt >= NG_SOCK_IOVMAX) {
#ifdef NG_DEBUG
ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 1, "Too many fragments");
#endif
#ifdef NG_IPSTATS_SUPPORTED
          ngUdpStat.udps_ierrors++;
#endif
          goto drop;
        }
        ngIpTmpIov[f_iovcnt].iov_base = f_bufp->buf_datap;
        ngIpTmpIov[f_iovcnt].iov_len = f_bufp->buf_datalen;
        f_iovcnt++;
        f_bufp = f_bufp->buf_next;
      }
      uh->uh_sum = ngIpCksum( ip, bufp->buf_datalen + sizeof( NGudpiphdr),
                             ngIpTmpIov, f_iovcnt);
    }
    else {
      uh->uh_sum = ngIpCksum( ip, len + sizeof( NGiphdr), NULL, 0);
    }
#else
    uh->uh_sum = ngIpCksum( ip, len + sizeof( NGiphdr), NULL, 0);
#endif
    if( uh->uh_sum != 0) {
#ifdef NG_DEBUG
      ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 1, "Bad checksum");
#endif
#ifdef NG_IPSTATS_SUPPORTED
      ngUdpStat.udps_ierrors++;
#endif
      goto drop;
    }
  }

  /*
   * find a destination socket
   */
  /* test for direct interface first */
  if( (uh->uh_dport == ngUdp_DirectPort) &&
    ngUdp_DirectPort && ngUdp_DirectInput_f ) {
      direct = 1;
  }
  /* multicast/broadcast */
  else if( NG_IN_MULTICAST( ip->ip_dst) ||
       (ip->ip_dst == NG_INADDR_BROADCAST) ||
       ((((NGifnet *) bufp->buf_ifnetp)->if_flags & NG_IFF_BROADCAST) &&
       ((ip->ip_dst == ((NGifnet *) bufp->buf_ifnetp)->if_netbroadcast) ||
        (ip->ip_dst == ((NGifnet *) bufp->buf_ifnetp)->if_broadaddr)) ) ) {
    direct = 2;
  }
  /* unicast */
  else {
    direct = 0;
    so = ngSoLookup( (NGsock *) &ngUdp_Sockq,
     ip->ip_dst, uh->uh_dport, ip->ip_src, uh->uh_sport, 1);
  }

  if( so || direct) {
#ifdef NG_IPSTATS_SUPPORTED
    ngUdpStat.udps_ipackets++;
#endif
    
  if( so == NULL) {
	ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 1, "NULL pointer, return from udpInput()"); 
    return;
    }
	
    if( !direct && NG_QUEUE_FULL( &so->so_rcv_q)) {
#ifdef NG_DEBUG
        ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 1, "Socket input queue full");
#endif
        goto drop;
    }
    /* adjust message length = udp data only */
    len -= sizeof( NGudphdr);
#ifdef XXX
    /* copy source address and port before udp data */
    addr.sin_addr = ip->ip_src;
    addr.sin_port = uh->uh_sport;
    ngMemCpy( bufp->buf_datap - sizeof( NGsockaddr), &addr,
              sizeof( NGsockaddr));
#endif
    /* save src & dst addresses */
    ip_src = ip->ip_src;
    ip_dst = ip->ip_dst;
    uh_sport = uh->uh_sport;
    uh_dport = uh->uh_dport;
    /* store addresses in buffer (replacing ip header) */
    ((NGsockbufaddr *) ip)->sba_src.sin_len = sizeof( NGsockaddr);
    ((NGsockbufaddr *) ip)->sba_src.sin_family = NG_AF_INET;
    ((NGsockbufaddr *) ip)->sba_src.sin_addr = ip_src;
    ((NGsockbufaddr *) ip)->sba_src.sin_port = uh_sport;
    ((NGsockbufaddr *) ip)->sba_dst = ip_dst;
    bufp->buf_iov = (NGiovec *) ip;

    if( direct == 1) {
      /* send directly to direct interface */
      (ngUdp_DirectInput_f)( bufp, (NGsockaddr *) bufp->buf_iov);
#ifdef NG_DEBUG
      ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 0, "Direct Interface");
#endif
    }
    else if( direct == 2) {
      /* multicast/broadcast destination */
      /* datagram must be copyed to all recipients */
      NGsock *lastso;

      lastso = NULL;

      for( so = (NGsock *) ngUdp_Sockq.next;
           so != (NGsock *) &ngUdp_Sockq;
           so = (NGsock *) so->so_node.next ) {
        /* check destination port*/
        if( so->so_lport != uh_dport)
          continue;
        if( (so->so_laddr != NG_INADDR_ANY) &&
            (so->so_laddr != ip_dst))
          continue;
        if( (so->so_faddr != NG_INADDR_ANY) &&
            ( (so->so_faddr != ip_src) ||
              (so->so_fport != uh_sport)) )
          continue;
        if( lastso) {
          /* copy data in receive buffer */
          if( !NG_QUEUE_FULL( &lastso->so_rcv_q)) {
            NGbuf *nbufp;
            /* allocate a new list of buffers */
            nbufp = ngBufCopy( bufp);
            if( nbufp) {
              nbufp->buf_iov = bufp->buf_iov;
              lastso->so_rcv_cc += len;
              NG_QUEUE_IN( &lastso->so_rcv_q, nbufp);
              ngSoDataReceived( lastso);
            }
          }
        }
        lastso = so;
        if( (lastso->so_options & (NG_SO_REUSEPORT|NG_SO_REUSEADDR))
             == 0) break; /* no need to look for other matches */
      }
      if( lastso) {
        /* add data in receive buffer */
        if( !NG_QUEUE_FULL( &lastso->so_rcv_q)) {
          lastso->so_rcv_cc += len;
          NG_QUEUE_IN( &lastso->so_rcv_q, bufp);
          ngSoDataReceived( lastso);
        }
        else goto drop;
        return;
      }
#ifdef NG_IPSTATS_SUPPORTED
      ngUdpStat.udps_noport++;
#endif
      goto drop;
    }
    else {
      /* add message in receive buffer */
      /*lint -e613 (Warning -- Possible use of null pointer) */
      so->so_rcv_cc += len;
      NG_QUEUE_IN( &so->so_rcv_q, bufp);
      /*lint +e613 (Warning -- Possible use of null pointer) */
      ngSoDataReceived( so);
#ifdef NG_DEBUG
      ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 0, "Socket found");
#endif
    }
    return;
  }
#ifdef NG_DEBUG
  ngDebug( NG_DBG_UDP, NG_DBG_INPUT, 0, "No match");
#endif
#ifdef NG_IPSTATS_SUPPORTED
  ngUdpStat.udps_noport++;
#endif
drop:
#ifdef NG_IPFRAG_SUPPORTED
  if( bufp->buf_flags & NG_BUFF_MOREDATA) {
    while( bufp) {
      f_bufp = bufp->buf_next;
      ngBufFree( bufp);
      bufp = f_bufp;
    }
    return;
  }
#endif
  ngBufFree( bufp);
  return;
}

/*****************************************************************************
 * udpUser()
 *****************************************************************************
 * Process user requests
 *****************************************************************************
 */

static int udpUser( NGsock *so, int code, void *data, int len, NGsockaddr *addr)
{
  int err;
  NGbuf *bufp;
  NGuint laddr, daddr;
  NGushort dport;
  NGudpiphdr *ui;
  int i, dlen, flags;

  err = NG_EOK;
  daddr = 0;
  dport = 0;
  laddr = 0;

  switch( code) {

  /*
   * ATTACH
   * Attach a new socket to the protocol
   * initialize socket structure
   */
  case NG_PRU_ATTACH:
    /* add socket to the list */
    NG_NODE_IN( &ngUdp_Sockq, so);
    /* input messages queue */
    NG_QUEUE_INIT( &so->so_rcv_q, NG_UDP_INPQMAX);
    so->so_rcv_hiwat = so->so_rcv_q.qu_nelmax*( ngBufDataMax
                        - sizeof( NGudphdr) - sizeof( NGiphdr));
    so->so_rcv_lowat = NG_SOCK_RECV_LOWAT;
    /* output messages maximum size */
    so->so_snd_hiwat = 4*(ngBufDataMax - sizeof( NGudphdr) - sizeof( NGiphdr));
    so->so_snd_lowat = NG_SOCK_SEND_LOWAT;
    break;

  /*
   * DETACH
   * Detach a socket from the protocol
   * Free all data associated with the socket
   */
  case NG_PRU_DETACH:
    /* remove socket from the list */
    NG_NODE_DETACH( so);
    break;

  /*
   * SHUTDOWN
   * stop sending data
   */
  case NG_PRU_SHUTDOWN:
    ngSoCantSendMore( so);
    break;

  /*
   * BIND
   * Assign a local address and port to the socket
   */
  case NG_PRU_BIND:
    err = ngSoBind( so, addr, (NGsock *) &ngUdp_Sockq, &udpPort);
    break;

  /*
   * CONNECT
   * Assign a foreign address and port to the socket
   */
  case NG_PRU_CONNECT:
    if( so->so_state & NG_SS_ISCONNECTED) {
      /* first disconnect 14/02/2001 */
      so->so_faddr = NG_INADDR_ANY;
      so->so_fport = 0;
      so->so_laddr = NG_INADDR_ANY;
      so->so_state &= ~NG_SS_ISCONNECTED;
    }
    err = ngSoConnect( so, addr, (NGsock *) &ngUdp_Sockq, &udpPort);
    if( !err)so->so_state |= NG_SS_ISCONNECTED;
    break;

  /*
   * SEND/SENDBUF
   * Send data
   * For non-connected sockets a temporarily connection is made
   */
  case NG_PRU_SEND:
  case NG_PRU_SENDBUF:
  	
    /* send udp datagram */
    if( so) {
      /* from socket interface */
      if( addr) {
        /* temporarily connect the socket */
        if( so->so_faddr != NG_INADDR_ANY) {
          /* socket already connected */
          err = NG_EISCONN;
          break;
        }
        laddr = so->so_laddr;
        err = ngSoConnect( so, addr, (NGsock *) &ngUdp_Sockq, &udpPort);
        if( err) {
          break;
        }
      }
      else if( so->so_faddr == NG_INADDR_ANY) {
        err = NG_ENOTCONN;
        break;
      }
    }
    else {
      /* from direct interface */
      if( addr) {
        /* store destination address */
        daddr = addr->sin_addr;
        dport = addr->sin_port;
      }
      else {
        /* a destination must be passed */
        err = NG_ENOTCONN;
        if (code == NG_PRU_SENDBUF) ngBufFree((NGbuf*)data);
        break;
      }
    }
    /* allocate message buffer */
    if( code == NG_PRU_SEND) {
      ngBufAlloc( bufp);
      if( bufp == NULL) {
        err = NG_ENOBUFS;
        goto send_end;
      }

      /* prepare buffer structure */
      bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset;
      bufp->buf_datalen = sizeof( NGudpiphdr);
      bufp->buf_iov = data;
      bufp->buf_iovcnt = len;
      bufp->buf_flags |= NG_BUFF_EXTRADATA;
      /* length of data */
      dlen = 0;
      for( i=0; i<len; i++) dlen += ((NGiovec *) data)[i].iov_len;
    }
    else {
      /* zero copy interface */
      /* data already in buffer */
      /* just add udp/ip headers */
      bufp = data;
      dlen = bufp->buf_datalen;
      bufp->buf_datap -= sizeof( NGudpiphdr);
      bufp->buf_datalen += sizeof( NGudpiphdr);
      bufp->buf_iov = NULL;
      bufp->buf_iovcnt = 0;
    }

    /* pseudo ip-udp header */
    ui = (NGudpiphdr *) bufp->buf_datap;
    ui->ui_zero1 = 0;
    ui->ui_zero2 = 0;
    ui->ui_zero3 = 0;
    ui->ui_pr = IPPROTO_UDP;
    ui->ui_len = ngHTONS( (NGushort) (dlen + sizeof( NGudphdr)));
    if( so) {
      /* addresses from socket */
      ui->ui_src = so->so_laddr;
      ui->ui_sport = so->so_lport;
      ui->ui_dst = so->so_faddr;
      ui->ui_dport = so->so_fport;
    }
    else {
      NGuint gaddr;
      /* direct interface, find destination interface */
      if( !(bufp->buf_flags & NG_BUFF_SETIF)) {
        if( (bufp->buf_ifnetp = ngIpRouteLookup( daddr, &gaddr, 0))
                 == NULL) {
          ngBufFree( bufp);
          err = NG_EHOSTUNREACH;
          goto send_end;
        }
      }
      ui->ui_src = ((NGifnet *) bufp->buf_ifnetp)->if_addr;
      ui->ui_sport = ngUdp_DirectPort;
      ui->ui_dst = daddr;
      ui->ui_dport = dport;
    }
    ui->ui_ulen = ui->ui_len;
    ui->ui_sum = 0;

    /* compute UDP checksum */
    if( ngIp_flags & NG_UDPO_CHECKSUM) {
      ui->ui_sum = ngIpCksum( bufp->buf_datap, bufp->buf_datalen,
                              bufp->buf_iov, bufp->buf_iovcnt);
      if( ui->ui_sum == 0) ui->ui_sum = 0xffff;
    }

    /* Send datagram to ip level */
    /* Prepare IP header */
    flags = 0;
    ((NGiphdr *) ui)->ip_len = sizeof( NGudpiphdr) + dlen;
    ((NGiphdr *) ui)->ip_hlv = sizeof( NGiphdr)>>2;
    if( so) {
      if( NG_IN_CLASSD( ui->ui_dst))
        ((NGiphdr *) ui)->ip_ttl = so->so_mcast_ttl;
      else
        ((NGiphdr *) ui)->ip_ttl = so->so_ttl;
      ((NGiphdr *) ui)->ip_tos = so->so_tos;
      /* socket options */
      if( so->so_options & NG_SO_DONTFRAG)
        ((NGiphdr *) ui)->ip_off = IPOFF_DF;
      if( so->so_options & NG_SO_DONTROUTE)
        flags |= NG_IPOUTPUT_DONTROUTE;
      if( so->so_options & NG_SO_MCASTLOOP)
        flags |= NG_IPOUTPUT_MCASTLOOP;
    }
    else {
      ((NGiphdr *) ui)->ip_ttl = ngIp_ttl;
      ((NGiphdr *) ui)->ip_tos = ngIp_tos;
    }

    /* set output interface ? */
    if( so && so->so_mcast_ifnetp && ((ui->ui_dst == NG_INADDR_ANY) ||
      NG_IN_CLASSD( ui->ui_dst)) ) {
      bufp->buf_flags |= NG_BUFF_SETIF;
      bufp->buf_ifnetp = so->so_mcast_ifnetp;
    }

    /* send buffer */
#ifdef NG_IPSTATS_SUPPORTED
    ngUdpStat.udps_opackets++;
#endif
    err = ngIpOutput( bufp, flags);

    if( !err) err = dlen; /* number of bytes sent */

send_end:
    if( so && addr) {
      /* disconnect socket */
      so->so_faddr = NG_INADDR_ANY;
      so->so_fport = 0;
      so->so_laddr = laddr;
      so->so_state &= ~NG_SS_ISCONNECTED;
    }
    break;

  /*
   * SENDBUFINI
   * Prepare a buffer for zero copy processing
   */
  case NG_PRU_SENDBUFINI:
   /* set the buffer data pointers */
   ((NGbuf *) data)->buf_datap = ((NGubyte *) data) + ngBufDataOffset
                                                    + sizeof( NGudpiphdr);
   ((NGbuf *) data)->buf_datalen = ngBufDataMax - sizeof( NGudpiphdr);
    break;
  /*
   * Unsupported commands
   */
  case NG_PRU_ABORT:
  case NG_PRU_LISTEN:
  case NG_PRU_RCVD:
  case NG_PRU_RCVOOB:
  case NG_PRU_SENDOOB:
  default:
    err = NG_EOPNOTSUPP;
  }

  return( err);
}

/*****************************************************************************
 * ngProto_UDP()
 *****************************************************************************
 * Protocol structure definition
 *****************************************************************************
 */

const NGproto ngProto_UDP = {
  "UDP",
  NG_SOCK_DGRAM,
  NG_PR_ADDR|NG_PR_ATOMIC,
  NG_PROTO_UDP,
  udpInit,
  NULL,
  NULL,
  udpInput,
  NULL,
  NULL,
  udpUser,
};

