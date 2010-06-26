/*****************************************************************************
 * $Id: sio_send.c,v 1.3 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOSend()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOSend()
 *****************************************************************************
 * Send data to a socket
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  buf     send buffer
 *  buflen  size of buffer
 *  flags   flags
 *  from    target address or NULL for connected sockets
 * Return value: number of bytes sent or negative error code
 */

int ngSAIOSend( NGsock *so, void *buf, int buflen, int flags,
                NGsockaddr *addr)
{
  NGiovec iov;

  /* prepare iovec structure */
  iov.iov_base = buf;
  iov.iov_len = buflen;

  /* call generic function */
  return( ngSAIOSendv( so, &iov, 1, flags, addr));
}

