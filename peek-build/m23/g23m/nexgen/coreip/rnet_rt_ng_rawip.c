/*****************************************************************************
 * $Id: rawip.c,v 1.5 2001/05/28 16:28:23 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Raw socket interface
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998,1999,2000 NexGen Software.
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
 * rawipInit()
 * rawipInput()
 * rawipUser()
 * ngProto_RAWIP
 *----------------------------------------------------------------------------
 * 28/05/98 - Regis Feneon
 * 05/01/2000 - Herve Ruault
 *  adjust bufp->datap (rawipUser() lines 262 and 290)
 *  adjust ip->ip_len  (rawipUser() line 320)
 * 18/02/2000 - Adrien Felon
 *  ipInput(): adding argument "flag" for ngIpOutput() call
 * 07/04/2000 -
 *  free buffer to send via zero copy interface if any error in rawipUser()
 * 01/09/2000 - Regis Feneon
 *  rawipInput(): removed lockup when the first socket is not the right one
 * 08/09/2000 -
 *  rawipUser/NG_PRU_SEND
 *  now clear ip_off field (IPOFF_DF flag) when contructing IP header
 * 16/11/2000 -
 *  added IP statistics
 * 24/11/2000 -
 *  new ngProtoList & NGproto structure
 *  rawipInput() - now copy message to all recipients
 *                 use ip->ip_len as message length
 *  rawipUser()/NG_PRU_SEND - support for DONTFRAG option
 * 23/01/2001 -
 *  corrections in NGiphdr structure
 * 14/02/2001 -
 *  allow re-connection and disconnection of socket
 *  removed NG_PRU_DISCONNECT
 * 18/02/2001 -
 *  rawipUser()/PRU_SEND - pass DONTROUTE & MCASTLOOP flags to ngIpOutput()
 * 19/04/2001 -
 *  rawipUser()/PRU_SEND - dont return ENOTCONN if HDRINCL option is set
 *   and no address is provided
 * 28/05/2001 -
 *  global data moved to rawip_dt.c
 *  uses ngIpTmpIov[]
 *****************************************************************************/

#include <ngip.h>
#include <ngip/ip.h>
#include <ngdsock.h>

#define NG_RAWIP_INPQMAX 4

/*****************************************************************************
 * rawipInit()
 *****************************************************************************
 * Protocol Initialization
 *****************************************************************************
 */

static void rawipInit( void)
{
  /* initialize socket list */
  ngRawip_Sockq.next = ngRawip_Sockq.prev = &ngRawip_Sockq;

}

/*****************************************************************************
 * rawipInput()
 *****************************************************************************
 * Process input messages
 *****************************************************************************
 */

static void rawipInput( NGbuf *bufp)
{
  NGiphdr *ip;
  NGsock *lastso, *tso;
#ifdef NG_IPFRAG_SUPPORTED
  NGbuf *f_bufp;
#endif

  /* pointer to IP header */
  ip = (NGiphdr *) bufp->buf_datap;

  /*
   * find a destination socket
   */
  lastso = NULL;
  tso = (NGsock *) ngRawip_Sockq.next;
  while( tso != (NGsock *) &ngRawip_Sockq) {
    if( ((tso->so_ipproto == ip->ip_p) || (tso->so_ipproto == 0)) &&
        ((tso->so_laddr == ip->ip_dst) || (tso->so_laddr == INADDR_ANY)) &&
        ((tso->so_faddr == ip->ip_src) || (tso->so_faddr == INADDR_ANY)) ) {
      if( lastso &&
          ((lastso->so_rcv_cc + ip->ip_len) <= lastso->so_rcv_hiwat) &&
          !NG_QUEUE_FULL( &lastso->so_rcv_q)) {
        NGbuf *tbufp;
        /* copy datagram */
        tbufp = ngBufCopy( bufp);
        if( tbufp != NULL) {
          lastso->so_rcv_cc += ip->ip_len;
          NG_QUEUE_IN( &lastso->so_rcv_q, tbufp);
          ngSoDataReceived( lastso);
        }
      }
      lastso = tso;
    }
    /* next socket (01/09/2000) */
    tso = (NGsock *) tso->so_node.next;
  }

  if( lastso) {
    /* (at least one) destination socket found */
    /* add message in socket receive buffer */
    /* if input buffer not full */
    if( ((lastso->so_rcv_cc + ip->ip_len) <= lastso->so_rcv_hiwat) &&
        !NG_QUEUE_FULL( &lastso->so_rcv_q)) {
      lastso->so_rcv_cc += ip->ip_len;
      NG_QUEUE_IN( &lastso->so_rcv_q, bufp);
      ngSoDataReceived( lastso);
#ifdef NG_DEBUG
      ngDebug( NG_DBG_RAWIP, NG_DBG_INPUT, 0, "Socket found");
#endif
      return;
    }

  }
#ifdef NG_IPSTATS_SUPPORTED
  else {
    /* no listener for this protocol */
    ngIpStat.ips_delivered--;
#ifdef NG_IGMP_SUPPORTED
    if( (ip->ip_p != IPPROTO_ICMP) &&
        (ip->ip_p != IPPROTO_IGMP))
#else
    if( ip->ip_p != IPROTO_ICMP)
#endif
    {
      ngIpStat.ips_noproto++;
    }
    else {
      ngIpStat.ips_idrops++;
    }
  }
#endif

  /* discard buffer */
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
 * rawipUser()
 *****************************************************************************
 * Process user requests
 *****************************************************************************
 */

extern NGushort ngIpId;

static int rawipUser( NGsock *so, int code, void *data, int len, NGsockaddr *addr)
{
  int err;
  NGbuf *bufp;
  NGifnet *netp;
  NGiphdr *ip;
  int i, dlen, hlen, flags;

  err = NG_EOK;

  switch( code) {

  /*
   * ATTACH
   * Attach a new socket to the protocol
   * initialize socket structure
   */
  case NG_PRU_ATTACH:
    /* add socket to the list */
    NG_NODE_IN( &ngRawip_Sockq, so);
    /* protocol number (passed in len) */
    so->so_ipproto = len;
    /* input messages queue */
    NG_QUEUE_INIT( &so->so_rcv_q, NG_RAWIP_INPQMAX);
    so->so_rcv_hiwat = so->so_rcv_q.qu_nelmax*4*ngBufDataMax;
    so->so_rcv_lowat = NG_SOCK_RECV_LOWAT;
    /* output messages maximum size */
    so->so_snd_hiwat = 4*ngBufDataMax;
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
    /* local address must match an interface address */
    netp = ngIfList;
    while( netp != NULL) {
      if( netp->if_addr == addr->sin_addr) break;
      netp = netp->if_next;
    }
    if( netp == NULL) {
      err = NG_EADDRNOTAVAIL;
      break;
    }
    so->so_laddr = addr->sin_addr;
    break;

  /*
   * CONNECT
   * Assign a foreign address to the socket
   */
  case NG_PRU_CONNECT:
    if( so->so_state & NG_SS_ISCONNECTED) {
      /* disconnect first */
      so->so_faddr = NG_INADDR_ANY;
      so->so_state = ~NG_SS_ISCONNECTED;
    }
    if( so->so_faddr != NG_INADDR_ANY) {
      so->so_faddr = addr->sin_addr;
      so->so_state |= NG_SS_ISCONNECTED;
    }
    break;

  /*
   * SEND/SENDBUF
   * Send data
   */
  case NG_PRU_SEND:
  case NG_PRU_SENDBUF:

    /* if not connected we need a destination address */
    if( !(so->so_options & NG_SO_HDRINCL) &&
         (so->so_faddr == NG_INADDR_ANY) && (addr == NULL)) {
      err = NG_ENOTCONN;
      if( code == NG_PRU_SENDBUF) ngBufFree((NGbuf*)data);
      break;
    }

    if( code == NG_PRU_SENDBUF) {
      /* zero copy interface */
      bufp = (NGbuf *) data;
      dlen = bufp->buf_datalen;
    }
    else {
      /* length of data */
      dlen = 0;
      for( i=0; i<len; i++) dlen += ((NGiovec *)data)[i].iov_len;
      /* allocate buffer */
      ngBufAlloc( bufp);
      if( bufp == NULL) {
        err = NG_ENOBUFS;
        break;
      }

      if( so->so_options & NG_SO_HDRINCL) {
        /* copy IP header into buffer */
        bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset;
        /* 5/1/2000 "- sizeof( NGiphdr)" removed */
        bufp->buf_datalen = sizeof( NGiphdr);
        ngIovCpy( bufp->buf_datap, data, len, 0, sizeof( NGiphdr));
        hlen = (((NGiphdr *) bufp->buf_datap)->ip_hlv &
                 NG_IPHDRLEN_MASK)<<2;
        if( (unsigned)hlen > sizeof( NGiphdr)) {
          /* copy IP options */
          ngIovCpy( bufp->buf_datap + sizeof( NGiphdr),
              (NGiovec *) data, len, sizeof( NGiphdr),
              hlen - sizeof( NGiphdr));
        }
        bufp->buf_datalen = hlen;
        /* make iovec array */
        bufp->buf_iovcnt = 0;
        bufp->buf_iov = &ngIpTmpIov[0];
        for( i=0; i<len; i++) {
          if( hlen < ((NGiovec *) data)[i].iov_len) {
            ngIpTmpIov[bufp->buf_iovcnt].iov_len =
                ((NGiovec *) data)[i].iov_len - hlen;
            ngIpTmpIov[bufp->buf_iovcnt].iov_base =
                ((NGubyte *) ((NGiovec *) data)[i].iov_base) + hlen;
            bufp->buf_iovcnt++;
            hlen = 0;
          }
          else hlen -= ((NGiovec *) data)[i].iov_len;
          if( bufp->buf_iovcnt >= NG_SOCK_IOVMAX) break;
        }
      }
      else {
        bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset
                   + sizeof( NGiphdr);
        /* 5/1/2000 "+ sizeof( NGiphdr);" added */
        bufp->buf_datalen = 0;
        bufp->buf_iov = (NGiovec *) data;
        bufp->buf_iovcnt = len;
      }
      bufp->buf_flags |= NG_BUFF_EXTRADATA;
    }
    /* IP header */
    if( so->so_options & NG_SO_HDRINCL) {
      /* ip header included in data */
      ip = (NGiphdr *) bufp->buf_datap;
      if( ip->ip_id == 0) {
        /* set ip id if null */
        ip->ip_id = ngIpId++;
        ip->ip_id = ngHTONS( ip->ip_id);
      }
      bufp->buf_flags |= NG_BUFF_RAW;
    }
    else {
      /* prepend ip header */

      bufp->buf_datap -= sizeof( NGiphdr);
      bufp->buf_datalen += sizeof( NGiphdr);
      ip = (NGiphdr *) bufp->buf_datap;
      ip->ip_tos = so->so_tos;
      ip->ip_ttl = so->so_ttl;
      ip->ip_p = so->so_ipproto;
      ip->ip_hlv = sizeof( NGiphdr)>>2;
      ip->ip_len = sizeof( NGiphdr) + dlen;
      ip->ip_off = (so->so_options & NG_SO_DONTFRAG) ? IPOFF_DF : 0;
      ip->ip_src = so->so_laddr;
      if( so->so_faddr == INADDR_ANY) ip->ip_dst = addr->sin_addr;
      else ip->ip_dst = so->so_faddr;
      bufp->buf_flags &= ~NG_BUFF_RAW;
    }
    /* set ip output flags */
    flags = 0;
    if( so->so_options & NG_SO_DONTROUTE)
      flags |= NG_IPOUTPUT_DONTROUTE;
    if( so->so_options & NG_SO_MCASTLOOP)
      flags |= NG_IPOUTPUT_MCASTLOOP;
    /* send data */
    err = ngIpOutput( bufp, flags);
    if( !err) err = dlen; /* number of bytes sent */
    break;

  /*
   * SENDBUFINI
   * Prepare a buffer for zero copy processing
   */
  case NG_PRU_SENDBUFINI:
    /* set the buffer data pointers */
    ((NGbuf *) data)->buf_datap = ((NGubyte *) data) + ngBufDataOffset;
    ((NGbuf *) data)->buf_datalen = ngBufDataMax;
    if( (so->so_options & NG_SO_HDRINCL) == 0) {
      /* let space for IP header */
      ((NGbuf *) data)->buf_datap += sizeof( NGiphdr);
      ((NGbuf *) data)->buf_datalen -= sizeof( NGiphdr);
    }
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
 * ngProto_RAWIP()
 *****************************************************************************
 * Protocol structure definition
 *****************************************************************************
 */

const NGproto ngProto_RAWIP = {
  "RAWIP",
  NG_SOCK_RAW,
  NG_PR_ATOMIC|NG_PR_ADDR,
  NG_PROTO_RAWIP,
  rawipInit,
  NULL,
  NULL,
  rawipInput,
  NULL,
  NULL,
  rawipUser
};

