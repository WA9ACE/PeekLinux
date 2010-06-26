/*****************************************************************************
 * $Id: sio_sndv.c,v 1.8 2001/06/08 13:14:31 rf Exp $
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
 * ngSAIOSendv()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from ngSoSend()
 *  removed 'gotos'...
 * 18/02/2001 -
 *  process NG_IO_DONTROUTE flag
 * 20/02/2001 -
 *  corrected test for buffer space
 *  added NG_IO_OOB flag processing
 * 23/05/2001 - David Girault
 *  does not wait place for a full buffer, reconstruct iov according free
 *  data in socket buffer
 * 28/05/2001 - RF
 *  special case for "atomic" protocols udp/rawip
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOSendv()
 *****************************************************************************
 * Send data to a socket
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  iov     iovect array
 *  iovcnt  number of iovect elements
 *  flags   flags
 *  from    target address or NULL for connected sockets
 * Return value: number of bytes sent or negative error code
 */

int ngSAIOSendv( NGsock *so, NGiovec *iov, int iovcnt, int flags,
                 NGsockaddr *to)
{
  int i, ret, totsend;
  int totlen, dontroute;
  int count, free;
#ifndef NG_RTOS
  u_long timeo = 0;
#endif

  /* data used in iov reconstruction */
  int tmpIovCur;
  int tmpIovOfs;

  /* compute message total length */
  totlen = 0; for( i=0; i<iovcnt; i++) totlen += iov[i].iov_len;
  totsend = 0;

  if( so->so_state & NG_SS_CANTSENDMORE) {
    /* connection closed */
    return( NG_EPIPE);
  }
  if( so->so_error) {
    /* asynchronous error */
    return( so->so_error);
  }

  /* UDP/RAWIP */
  if( ((NGproto *) so->so_proto)->pr_flags & NG_PR_ATOMIC) {

    if( flags & NG_IO_OOB)
      return( NG_EOPNOTSUPP);

    /* check for message too big */
    if( totlen > so->so_snd_hiwat)
      return( NG_EMSGSIZE);

    /* set dontroute flag for this message ? */
    if( (flags & NG_IO_DONTROUTE) && !(so->so_options & NG_SO_DONTROUTE)) {
      dontroute = 1;
      so->so_options |= NG_SO_DONTROUTE;
    }
    else dontroute = 0;

    /* call protocol send function */
    ret = NG_SOCKCALL_USER( so, NG_PRU_SEND, iov, iovcnt, to);

    /* restore options */
    if( dontroute) so->so_options &= ~NG_SO_DONTROUTE;

    return( ret);

  }

  /* TCP */

  /* Init variables */
  tmpIovCur = 0;
  tmpIovOfs = 0;

  /* Send Loop*/
  while (totlen) {

    /* check if more byte to send that we can now */
    free = (so->so_snd_hiwat - so->so_snd_cc);
    if( (totlen > free) && (free < so->so_snd_lowat)) {

      /* cannot block ? */
      if( (flags & NG_IO_DONTWAIT) ||
          (so->so_state & (NG_SS_NBIO|NG_SS_UPCALL)) ) {
        return( (totsend>0) ? totsend : NG_EWOULDBLOCK);
      }

#ifndef NG_RTOS
      /* 'arm' timer */
      if( so->so_snd_timeo) timeo = ngOSClockGetTime() + so->so_snd_timeo;
#endif
      /* wait for enougth space in send buffer */
      while( (so->so_error == 0) && ((so->so_state & NG_SS_CANTSENDMORE) == 0)
             && ( (so->so_snd_hiwat-so->so_snd_cc) < so->so_snd_lowat) ) {
#ifdef NG_RTOS
        if( ngSoSleepOnSend( so, so->so_snd_timeo) != 0) {
          return( (totsend>0) ? totsend : NG_EWOULDBLOCK);
        }
#else
        if( so->so_snd_timeo) {
          if( (long) (ngOSClockGetTime() - timeo) > 0) {
            return( (totsend>0) ? totsend : NG_EWOULDBLOCK);
          }
        }
        ngYield();
#endif
      }
    }

    if( so->so_state & NG_SS_CANTSENDMORE) {
      /* connection closed */
      return( NG_EPIPE);
    }
    else if( so->so_error) {
      /* asynchronous error */
      return( so->so_error);
    }
    else {

      /* Initialise local IOV according free space in queue */
      free = (so->so_snd_hiwat - so->so_snd_cc);
      count = 0;

      while ( (free > 0) && (tmpIovCur < iovcnt ) ) {
        ngIpTmpIov[count].iov_base = (void *)
          ((NGubyte *)iov[tmpIovCur].iov_base + tmpIovOfs);
        ngIpTmpIov[count].iov_len =
          NG_MIN( (iov[tmpIovCur].iov_len-tmpIovOfs), free );
        tmpIovOfs += ngIpTmpIov[count].iov_len;
        if ( tmpIovOfs >= iov[tmpIovCur].iov_len ) {
          tmpIovOfs = 0;
          tmpIovCur++;
        }
        free -= ngIpTmpIov[count].iov_len;
        count++;
      }

      /* call protocol send function */
      ret = NG_SOCKCALL_USER( so, ( (flags & NG_IO_OOB) ?
                                    NG_PRU_SENDOOB : NG_PRU_SEND ),
                              ngIpTmpIov, count, to);

      if (ret > 0) {
        /* update size */
        totsend += ret;
        totlen -= ret;
      }
      else {
        /* error, return */
        return( (totsend>0) ? totsend : ret);
      }

    }
  }
  return( totsend);
}

