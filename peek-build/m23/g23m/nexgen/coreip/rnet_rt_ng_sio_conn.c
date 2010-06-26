/*****************************************************************************
 * $Id: sio_conn.c,v 1.3 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOConnect()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from connect()
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOConnect()
 *****************************************************************************
 * Initiate connection on a socket
 *****************************************************************************
 * Parameters:
 *  so          pointer to socket control block
 *  addr        address to connect to
 *  flags       NG_IO_NONBLOCK: for non-blocking mode
 * Return value: NG_EOK or negative error code
 */

int ngSAIOConnect( NGsock *so, NGsockaddr *addr, int flags)
{
  int err;

  /* call protocol connect routine */
  err = NG_SOCKCALL_USER( so, NG_PRU_CONNECT, NULL, 0, addr);
  if( err) return( err);
  if( !(so->so_state & NG_SS_ISCONNECTED) &&
      ((so->so_state & (NG_SS_NBIO|NG_SS_UPCALL)) ||
       (flags & NG_IO_NONBLOCK)) ) {
    /* non-blocking */
    return( NG_EINPROGRESS);
  }

  /* wait for completion */
  while( !(so->so_state & NG_SS_ISCONNECTED) && !so->so_error) {
#ifdef NG_RTOS
    ngSoSleepOnCnct( so, 0);
#else
    ngYield();
#endif
  }
  /* check for error */
  if( so->so_error) {
    err = so->so_error;
    so->so_error = 0;
    return( err);
  }
  return( NG_EOK);
}

