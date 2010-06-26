/*****************************************************************************
 * $Id: sio_rcvv.c,v 1.5 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Asynchronous Sockets
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
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
 * ngSAIORecvv()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from ngSoRecv()
 *  removed 'gotos'...
 * 18/02/2001 -
 *  added dstaddr
 *  added NG_IO_OOB flag processing
 *****************************************************************************/

#include <ngip.h>
#include <ngip/ip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIORecv()
 *****************************************************************************
 * Receive data from a socket
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  iov     iovect array
 *  iovcnt  number of iovect elements
 *  flagsp  pointer to flags or NULL
 *  from    filled with sender address if not NULL
 *  dstaddr filled with destination IP address if not NULL
 * Return value: number of bytes received or negative error code
 */

int ngSAIORecvv( NGsock *so, NGiovec *iov, int iovcnt, int *flagsp,
                 NGsockaddr *from, NGuint *dstaddr)
{
  NGbuf *bufp, *tbufp;
  NGubyte *bufbase, *ciobase;
  int err, i, flags, firstbuf;
  u_int buflen, ciolen, nb, l;
  int totlen, bufread;
#ifndef NG_RTOS
  u_long timeo = 0;
#endif

  /* total amount of data requested */
  totlen = 0;
  for( i=0; i<iovcnt; i++) {
    if( iov[i].iov_len < 0) return( NG_EINVAL);
    totlen += iov[i].iov_len;
  }

  if( flagsp) flags = *flagsp;
  else flags = 0;

  /* must be connected to accept data */
  if( (((NGproto *) so->so_proto)->pr_flags & NG_PR_CONNREQUIRED) &&
    ((so->so_state & (NG_SS_ISCONNECTED|NG_SS_CANTRCVMORE)) == 0) ) {
    return( NG_ENOTCONN);
  }

  if( totlen == 0) {
    return( 0);
  }

  if( flags & NG_IO_OOB) {
    /* read Out-Of-Band data */
    err = NG_SOCKCALL_USER( so, NG_PRU_RCVOOB, iov[0].iov_base, flags, NULL);
    if( err == NG_EOK)
      return( 1); /* always read one byte of data */
    else
      return( err);
  }

  /* can't read more than maximum buffer size */
  if( totlen > so->so_rcv_hiwat) totlen = so->so_rcv_hiwat;

  /* get first buffer */
  NG_QUEUE_PEEK( &so->so_rcv_q, bufp);
  firstbuf = 1;

  /* if no data and error, return error code */
  if( (bufp == NULL) && so->so_error) {
    err = so->so_error;
    /* reset error code */
    if( (flags & NG_IO_PEEK) == 0) so->so_error = 0;
    return( err);
  }

  /* if no data and non-blocking mode then returns immediately */
  if( (bufp == NULL) && ((flags & NG_IO_DONTWAIT) ||
   (so->so_state & (NG_SS_NBIO|NG_SS_CANTRCVMORE|NG_SS_UPCALL))) ) {
    if( so->so_state & NG_SS_CANTRCVMORE) return( 0); /* connection is closed */
    return( NG_EWOULDBLOCK);
  }

#ifndef NG_RTOS
  /* 'arm' timer */
  if( so->so_rcv_timeo) timeo = ngOSClockGetTime() + so->so_rcv_timeo;
#endif

  while( 1) {

    /*
     * wait for data, conditions:
     * with NG_PR_ATOMIC, wait for one datagram
     * else if DONTWAIT and NBIO not set wait for min( rcv_lowat, totlen)
     * or if WAITALL is set wait for totlen data (maxi rcv_hiwat)
     */
    if( ( (so->so_state & NG_SS_CANTRCVMORE) == 0) && (
      ( (bufp == NULL) && (((NGproto *) so->so_proto)->pr_flags & NG_PR_ATOMIC) )
      || ( (((NGproto *) so->so_proto)->pr_flags & NG_PR_ATOMIC) == 0) &&
        ( ((so->so_rcv_cc < totlen) && (so->so_rcv_cc < so->so_rcv_lowat)
          && ((flags & NG_IO_DONTWAIT) == 0) &&
              ((so->so_state & (NG_SS_NBIO|NG_SS_UPCALL)) == 0))
         || ((so->so_rcv_cc < totlen) && (flags & NG_IO_WAITALL)) ) ) ) {
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

      if( bufp == NULL) {
        /* connection closed */
        if( so->so_error) {
          /* return error code if any */
          err = so->so_error;
          if( (flags & NG_IO_PEEK) == 0) so->so_error = 0;
          return( err);
        }
        return( 0);
      }

      /* datagram based protocol ? */
      if( ((NGproto *) so->so_proto)->pr_flags & NG_PR_ATOMIC) {
        int dlen = 0; /* size of datagram */
        tbufp = bufp;
        while( tbufp) {
          dlen += tbufp->buf_datalen;
          tbufp = tbufp->buf_next;
        }
        /* transfert min( totlen, dlen) */
        if( totlen < dlen) {
          /* data is truncated */
          if( flagsp) *flagsp |= NG_IO_TRUNC;
        }
        else totlen = dlen;
      }
      else if( totlen > so->so_rcv_cc) totlen = so->so_rcv_cc;

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
      else if( so->so_proto->pr_flags & NG_PR_ADDR) {
        if( from != NULL) {
          /* gets address in message */
          ngMemCpy( from, bufp->buf_iov, sizeof( NGsockaddr));
        }
        if( dstaddr != NULL) {
          /* idem */
          *dstaddr = ((NGsockbufaddr *) bufp->buf_iov)->sba_dst;
        }
      }

      nb = totlen; /* number of bytes to transfert */
      i = 0;
      ciobase = iov[0].iov_base;
      ciolen = iov[0].iov_len;
      while( bufp) {

        bufbase = bufp->buf_datap;
        buflen = bufp->buf_datalen;

        /* read one recv buffer or fill all user buffer */
        if( buflen > nb) buflen = nb;
        bufread = buflen;
        while( buflen > 0) {
          l = buflen > ciolen ? ciolen : buflen;
          ngMemCpy( ciobase, bufbase, l);
          ciolen -= l;
          if( ciolen == 0) {
            i++; /* next user buffer */
            ciobase = iov[i].iov_base;
            ciolen = iov[i].iov_len;
          }
          else ciobase += l;
          buflen -= l;
          bufbase += l;
          nb -= l;
        }

        /*
         * 'atomic' protocols: read one complete datagram
         */
        if( ((NGproto *) so->so_proto)->pr_flags & NG_PR_ATOMIC) {
          /* next data buffer */
          tbufp = bufp->buf_next;
          if( !(flags & NG_IO_PEEK)) {
            if( firstbuf) {
              NG_QUEUE_OUT( &so->so_rcv_q, bufp);
              firstbuf = 0;
            }
            if(bufp != NULL)
            {            
              so->so_rcv_cc -= bufp->buf_datalen;
              ngBufFree( bufp);
            }
          }
          else if( nb == 0) break;
        }
        else {
        /*
         * stream protocol: read next segment
         */
          tbufp = (NGbuf *) bufp->buf_node.next;
          if( !(flags & NG_IO_PEEK)) {
            so->so_rcv_cc -= bufread;
            if( bufread == bufp->buf_datalen) {
              /* free buffer */
              NG_QUEUE_OUT( &so->so_rcv_q, bufp);
              if(bufp != NULL)
              {            
                ngBufFree( bufp);
              }
            }
            else {
              /* adjust buffer pointers */
              bufp->buf_datap += bufread;
              bufp->buf_datalen -= bufread;
            }
          }
          if( nb == 0) break;
        }
        bufp = tbufp;

      } /* while( bufp) */

      /*
       * end of transfert
       */
      if( !(flags & NG_IO_PEEK) &&
           (((NGproto *) so->so_proto)->pr_flags & NG_PR_WANTRCVD)) {
          /* inform protocol we have read data */
          (void)NG_SOCKCALL_USER( so, NG_PRU_RCVD, NULL, 0, NULL);
      }
      return( totlen); /* returns number of bytes written */
    }

    if( bufp == NULL) NG_QUEUE_PEEK( &so->so_rcv_q, bufp);

  } /* while(1) */

}

