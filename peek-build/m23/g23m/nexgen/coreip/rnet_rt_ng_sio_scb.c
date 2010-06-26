/*****************************************************************************
 * $Id: sio_scb.c,v 1.4 2001/04/02 14:31:40 rf Exp $
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
 * ngSAIOSetCallback()
 *----------------------------------------------------------------------------
 * 15/02/2001 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOSetCallback()
 *****************************************************************************
 * Attach an event callback on a socket
 *****************************************************************************
 * Parameters:
 *  so          pointer to socket control block
 *  upcall_f    callback routine or NULL
 *  upcall_data data for upcall routine
 * Return value: NG_EOK or negative error code
 */

int ngSAIOSetCallback( NGsock *so, void (*upcall_f)( NGsock *, void *, int),
                       void *upcall_data)
{
  int flags;

  /* attach routine */
  so->so_upcall_f = upcall_f;
  so->so_upcall_data = upcall_data;

  /* events pending ? */
  if( (upcall_f != NULL) && !(so->so_state & NG_SS_UPCALL)) {
    if( (flags = ngSoGetEvents( so, NG_SIOSEL_READ|NG_SIOSEL_WRITE)) != 0) {
      so->so_state |= NG_SS_UPCALL;
      so->so_upcall_f( so, so->so_upcall_data, flags);
      so->so_state &= ~NG_SS_UPCALL;
    }
  }

  return( NG_EOK);
}

