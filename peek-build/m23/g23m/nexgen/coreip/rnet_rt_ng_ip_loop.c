/*****************************************************************************
 * $Id: ip_loop.c,v 1.3 2002/04/23 10:20:04 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet Protocol Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2002 NexGen Software.
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
 * ngIpLoopback()
 *----------------------------------------------------------------------------
 * 30/11/2000 - Regis Feneon
 * 22/03/2002 -
 *  call ngIfGenInput() instead of direct access to input queue
 *****************************************************************************/

#include <ngip.h>

/*****************************************************************************
 * ngIpLoopback()
 *****************************************************************************
 * Loopback a packet into the network input queue
 *****************************************************************************
 * Parameters:
 *  netp        pointer to interface
 *  bufp        message to send
 * Return value: none
 */

void ngIpLoopback( NGifnet *netp, NGbuf *bufp)
{
  NGbuf *tbufp;

  /* update statistics */
  netp->if_opackets++;
  if( bufp->buf_flags & (NG_BUFF_BCAST|NG_BUFF_MCAST))
    netp->if_omcasts++;
  netp->if_obytes += bufp->buf_datalen;

  /* if DONTFREE flag we must copy the message first */
  if( bufp->buf_flags & NG_BUFF_DONTFREE) {
    tbufp = ngBufCopy( bufp);
    if( tbufp == NULL) {
      netp->if_iqdrops++;
      ngBufOutputFree( bufp);
      return;
    }
    /* release original buffer */
    ngBufOutputFree( bufp);
    /* works on copy now */
    bufp = tbufp;
  }

  /* IP only */
  bufp->buf_flags &= ~NG_BUFF_PROTO;
  bufp->buf_flags |= NG_PROTO_IP;

  ngIfGenInput( netp, bufp, 0);

}

