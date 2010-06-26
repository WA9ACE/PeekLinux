/*****************************************************************************
 * $Id: sio_acce.c,v 1.2 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOAccept()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from ngSockAccept()
 *  removed gotos...
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOAccept()
 *****************************************************************************
 * accept a connection on a passive socket
 *****************************************************************************
 * Parameters:
 *  so      listening socket
 *  addr    if not null filled with peer's address
 *  flags   NG_IO_NONBLOCK: force non-blocking operation
 *  newso   filled with accepted socket
 * Return value: NG_EOK or negative error code
 */

int ngSAIOAccept( NGsock *so, NGsockaddr *addr, int flags, NGsock **newso)
{
  int err;

  /* can accept a connection ? */
  if( !(so->so_options & NG_SO_ACCEPTCONN)) {
    return( NG_EOPNOTSUPP);
  }
  if( so->so_error) {
    /* test for asynchronous error */
    err = so->so_error;
    so->so_error = 0;
    return( err);
  }
  /* non-blocking mode */
  if( (so->so_aqlen == 0) &&
      ((flags & NG_IO_NONBLOCK) || (so->so_state & (NG_SS_NBIO|NG_SS_UPCALL))) ) {
    return( NG_EWOULDBLOCK);
  }

  /* wait for connection */
  while( (so->so_aqlen == 0) && !(so->so_error)) {
#ifdef NG_RTOS
    ngSoSleepOnCnct( so, 0);
#else
    ngYield();
#endif
  }
  if( so->so_error) {
    err = so->so_error;
    so->so_error = 0;
    return( err);
  }
  /* get new socket from connection queue */
  *newso = so->so_acceptq;
  so->so_acceptq = so->so_acceptq->so_acceptq;
  so->so_aqlen--;
  so = *newso;
  so->so_head = NULL;
  so->so_acceptq = NULL;
  so->so_state &= ~NG_SS_NOFDREF;
  if( addr) {
    /* fill in foreign address */
    addr->sin_family = NG_AF_INET;
    addr->sin_len = sizeof( NGsockaddr);
    addr->sin_addr = so->so_faddr;
    addr->sin_port = so->so_fport;
  }

  return( NG_EOK);
}

