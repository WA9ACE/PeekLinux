/*****************************************************************************
 * $Id: sio_recv.c,v 1.4 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIORecv()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 * 18/02/2001 -
 *  added dstaddr
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIORecv()
 *****************************************************************************
 * Receive data from a socket
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  buf     receive buffer
 *  buflen  size of buffer
 *  flagsp  pointer to flags or NULL
 *  from    filled with sender address if not NULL
 *  dstaddr filled with destination IP address if not NULL
 * Return value: number of bytes received or negative error code
 */

int ngSAIORecv( NGsock *so, void *buf, int buflen, int *flags,
                NGsockaddr *from, NGuint *dstaddr)
{
  NGiovec iov;

  /* prepare iovec structure */
  iov.iov_base = buf;
  iov.iov_len = buflen;

  /* call generic function */
  return( ngSAIORecvv( so, &iov, 1, flags, from, dstaddr));
}

