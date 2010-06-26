/*****************************************************************************
 * $Id: sio_clos.c,v 1.5 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOClose()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from ngSockClose()
 * 21/02/2001 -
 *  implementation of linger option
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOClose()
 *****************************************************************************
 * Close a socket
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  flags   if NG_IO_NONBLOCK, dont block if LINGER option is set
 * Return value: NG_EOK or negative error code
 */

int ngSAIOClose( NGsock *so, int flags)
{
  NGsock *tso;
  int err, err2;
  u_long ltimeo;

  /* cannot be called from the upcall routine */
  if( so->so_state & NG_SS_UPCALL)
    return( NG_EOPNOTSUPP);

  /* remove pending connections */
  if( so->so_options & NG_SO_ACCEPTCONN) {
    while( so->so_acceptq0) {
      tso = so->so_acceptq0; /* detach socket */
      so->so_acceptq0 = tso->so_acceptq0;
      so->so_aq0len--;
      tso->so_head = NULL;
      tso->so_acceptq0 = NULL;
      (void)NG_SOCKCALL_USER( tso, NG_PRU_ABORT, NULL, 0, NULL);
    }
    while( so->so_acceptq) {
      tso = so->so_acceptq; /* detach socket */
      so->so_acceptq = tso->so_acceptq;
      so->so_aqlen--;
      tso->so_head = NULL;
      tso->so_acceptq = NULL;
      (void)NG_SOCKCALL_USER( tso, NG_PRU_ABORT, NULL, 0, NULL);
    }
  }

  err = NG_EOK;

  /* if connected, disconnect */
  if( (so->so_state & NG_SS_ISCONNECTED) &&
      (so->so_proto->pr_flags & NG_PR_CONNREQUIRED)) {
    if( !(so->so_state & NG_SS_ISDISCONNECTING)) {
      /* force disconnection */
      err = NG_SOCKCALL_USER( so, NG_PRU_DISCONNECT, NULL, 0, NULL);
      /* wait for completion */
      if( (err == NG_EOK) && (so->so_options & NG_SO_LINGER) &&
          !(so->so_state & NG_SS_NBIO) && !(flags & NG_IO_NONBLOCK)) {
#ifdef NG_RTOS
        ltimeo = (u_long)so->so_linger*ngOSClockGetFreq();
#else
        ltimeo = ngOSClockGetTime() + ((u_long)so->so_linger*ngOSClockGetFreq());
#endif
        while( so->so_state & NG_SS_ISCONNECTED) {
#ifdef NG_RTOS
          if( ngSoSleepOnCnct( so, ltimeo) != NG_EOK) {
            err = NG_EWOULDBLOCK;
            break;
          }
#else
          if( (long) (ngOSClockGetTime() - ltimeo) > 0) {
            err = NG_EWOULDBLOCK;
            break;
          }
          ngYield();
#endif
        }
      }
    }
  }

  /* detach descriptor from protocol */
  err2 = NG_SOCKCALL_USER( so, NG_PRU_DETACH, NULL, 0, NULL);
  if( err == NG_EOK) err = err2;

  /* free the descriptor */
  if( !(so->so_state & NG_SS_NOFDREF))
    ngSoFree( so);

  return( err);
}

