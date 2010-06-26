/*****************************************************************************
 * $Id: sio_balo.c,v 1.3 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOBufAlloc()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from ngSockBufAlloc()
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOBufAlloc()
 *****************************************************************************
 * Allocate a buffer for zero copy operations
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  bufp    filled with pointer to allocated buffer
 * Return value: NG_EOK or negative error code
 */

int ngSAIOBufAlloc( NGsock *so, NGbuf **bufp)
{
  NGbuf *tbufp;
  int err;

  ngBufAlloc( tbufp);
  if( tbufp == NULL) {
    /* not enougth system resources */
    return( NG_ENOBUFS);
  }
  /* setup data pointers */
  err = NG_SOCKCALL_USER( so, NG_PRU_SENDBUFINI, tbufp, 0, NULL);
  if( err != NG_EOK) {
    ngBufFree( tbufp);
  }
  else {
    *bufp = tbufp;
  }
  return( err);
}

