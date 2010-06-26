/*****************************************************************************
 * $Id: sio_brcv.c,v 1.5 2002/04/23 10:20:04 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Asynchronous Sockets
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
 * ngSAIOBufRecv()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from ngSockBufRecv()
 * 21/03/2001 -
 *  added dstaddr and corrected source address assignment
 * 09/04/2002 -
 *  corrected reception of fragmented data
 *****************************************************************************/

#include <ngip.h>
#include <ngip/ip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOBufRecv()
 *****************************************************************************
 * Receive a message buffer with zero copy
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  flagsp  in/out receive flags:
 *            NG_IO_DONTWAIT  force non-blocking operation
 *            NG_IO_TRUNC     part of data has been truncated
 *  from    if not null filled with sender address
 *  rbufp   filled with pointer to received buffer
 *  dstaddr if not null filled with destination address
 * Return value: NG_EOK or negative error code
 */

int ngSAIOBufRecv( NGsock *so, int *flagsp, NGsockaddr *from, NGbuf **rbufp,
                   NGuint *dstaddr)
{
  NGbuf *bufp, *tbufp;
  int flags;
#ifndef NG_RTOS
  u_long timeo = 0;
#endif

  if( flagsp) flags = *flagsp;
  else flags = 0;

  if( flags & (NG_IO_PEEK|NG_IO_OOB)) {
    return( NG_EINVAL);
  }

  /* datagrams protocols only */
  if( (((NGproto *) so->so_proto)->pr_flags & NG_PR_ATOMIC) == 0) {
    return( NG_EOPNOTSUPP);
  }

  /* get first buffer */
  NG_QUEUE_OUT( &so->so_rcv_q, bufp);

  /* if no data and non-blocking mode then returns immediately */
  if( (bufp == NULL) && ((flags & NG_IO_DONTWAIT) ||
   (so->so_state & (NG_SS_NBIO|NG_SS_UPCALL))) ) {
    return( NG_EWOULDBLOCK);
  }

#ifndef NG_RTOS
  if( so->so_rcv_timeo) timeo = ngOSClockGetTime() + so->so_rcv_timeo;
#endif

  while( 1) {

    /*
     * wait for data
     */
    if( bufp == NULL) {
#ifdef NG_RTOS
      if( ngSoSleepOnRecv( so, so->so_rcv_timeo) != 0) {
        return( NG_EWOULDBLOCK);
      }
#else
      if( so->so_rcv_timeo) {
        if( (long) (ngOSClockGetTime() - timeo) > 0) {
          return( NG_EWOULDBLOCK);
        }
      }
      ngYield();
#endif
    }
    else {
      /*
       * transfert data
       */
      /* discard fragmented data */
      if( bufp->buf_next) {
        if( flagsp) *flagsp |= NG_IO_TRUNC;
        /* free other buffers */
        tbufp = bufp->buf_next;
        while( tbufp) {
          NGbuf *tbufp2;
          /* adjust buffer byte count */
          so->so_rcv_cc -= tbufp->buf_datalen;
          tbufp2 = tbufp->buf_next;
          ngBufFree( tbufp);
          tbufp = tbufp2;
        }
      }
      so->so_rcv_cc -= bufp->buf_datalen;
      /* fill source & dest address */
      if( so->so_proto->pr_type == NG_SOCK_RAW) {
        /* buf_datap is containing a IP header */
        if( from != NULL) {
          from->sin_len = sizeof( NGsockaddr);
          from->sin_family = NG_AF_INET;
          from->sin_addr = ((NGiphdr *) bufp->buf_datap)->ip_src;
          from->sin_port = 0;
        }
        if( dstaddr != NULL) {
          *dstaddr = ((NGiphdr *) bufp->buf_datap)->ip_dst;
        }
      }
      else {
        if( from != NULL) {
          /* gets address in message */
          ngMemCpy( from, bufp->buf_iov, sizeof( NGsockaddr));
        }
        if( dstaddr != NULL) {
          /* idem */
          *dstaddr = ((NGsockbufaddr *) bufp->buf_iov)->sba_dst;
        }
      }
      /* return message buffer */
      *rbufp = bufp;
      return( NG_EOK);
    }
    NG_QUEUE_OUT( &so->so_rcv_q, bufp);

  } /* while() */

}

