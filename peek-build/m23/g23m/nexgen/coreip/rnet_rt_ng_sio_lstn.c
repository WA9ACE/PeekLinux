/*****************************************************************************
 * $Id: sio_lstn.c,v 1.3 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOListen()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from listen()
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOListen()
 *****************************************************************************
 * Listen for connections on a socket
 *****************************************************************************
 * Parameters:
 *  so          pointer to socket control block
 *  backlog     max number of pending connections
 * Return value: NG_EOK or negative error code
 */

int ngSAIOListen( NGsock *so, int backlog)
{
  int err;

  err = NG_SOCKCALL_USER( so, NG_PRU_LISTEN, NULL, 0, NULL);
  if( err) return( err);

  so->so_options |= NG_SO_ACCEPTCONN;
  so->so_aqmax = backlog > 0 ? backlog : 1;

  return( NG_EOK);

}

