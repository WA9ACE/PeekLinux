/*****************************************************************************
 * $Id: buf_copy.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Network Buffer Management
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
 * ngBufCopy()
 *----------------------------------------------------------------------------
 * 27/11/2000 - Regis Feneon
 *****************************************************************************/

#include <ngos.h>
#include <ngnet.h>

/*****************************************************************************
 * ngBufCopy()
 *****************************************************************************
 * Copy a list of network buffers
 *****************************************************************************
 * Parameters:
 *  bufp        network buffer to be copyed
 * Return value: the new buffer or NULL if no free buffer
 */

NGbuf *ngBufCopy( NGbuf *bufp)
{
  NGbuf *nbufp, *fbufp;

  /* allocate a new buffer */
  ngBufAlloc( nbufp);
  if( nbufp == NULL) return( NULL);
  /* copy flags */
  nbufp->buf_flags = bufp->buf_flags & (NG_BUFF_PROTO|NG_BUFF_RAW|
    NG_BUFF_MCAST|NG_BUFF_BCAST|NG_BUFF_UNALIGNED|NG_BUFF_SETIF);
  nbufp->buf_ifnetp = bufp->buf_ifnetp;
  /* copy data */
  fbufp = nbufp;
  while( 1) {
    /* initialize pointers */
    fbufp->buf_datap = ((NGubyte *) fbufp) +
                          (bufp->buf_datap - ((NGubyte *) bufp));
    fbufp->buf_datalen = bufp->buf_datalen;
    ngMemCpy( fbufp->buf_datap, bufp->buf_datap, bufp->buf_datalen);
    if( bufp->buf_flags & NG_BUFF_MOREDATA) {
      /* allocate new buffer */
      ngBufAlloc( fbufp->buf_next);
      if( fbufp->buf_next == NULL) goto freeall;
      fbufp->buf_flags |= NG_BUFF_MOREDATA;
      fbufp = fbufp->buf_next;
      bufp = bufp->buf_next;
    }
    else break;
  }

  return( nbufp);

freeall:
  while( nbufp) {
    fbufp = nbufp->buf_next;
    ngBufFree( nbufp);
    nbufp = fbufp;
  }
  return( NULL);
}

