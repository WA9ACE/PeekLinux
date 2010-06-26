/*****************************************************************************
 * $Id: so_getev.c,v 1.3 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Socket Level Internal Functions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1999-2001 NexGen Software.
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
 * ngSoGetEvents()
 *----------------------------------------------------------------------------
 * 15/02/2001 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSoGetEvents()
 *****************************************************************************
 * check for events on a sockets
 *****************************************************************************
 *  so          pointer to socket control block
 *  flags       events to be checked
 * Returne value: the current events
 */

int ngSoGetEvents( NGsock *so, int flags)
{
  int rflags;

  rflags = 0;

  /* reading ok ? */
  if( flags & NG_SIOSEL_READ) {
    if( (so->so_rcv_cc >= so->so_rcv_lowat) ||
        (so->so_state & NG_SS_CANTRCVMORE) ||
        (so->so_aqlen > 0) )
      rflags |= NG_SIOSEL_READ;
  }
  /* writting ok ? */
  if( flags & NG_SIOSEL_WRITE) {
    if( ( ((so->so_snd_hiwat - so->so_snd_cc) >= so->so_snd_lowat) &&
          ( (so->so_state & NG_SS_ISCONNECTED) ||
           !(so->so_proto->pr_flags & NG_PR_CONNREQUIRED) ) ) ||
        (so->so_state & NG_SS_CANTSENDMORE) )
      rflags |= NG_SIOSEL_WRITE;
  }
  /* oob at mark ? */
  if( flags & NG_SIOSEL_EXCEPT) {
    if( so->so_oobmark || (so->so_state & NG_SS_RCVATMARK))
      rflags |= NG_SIOSEL_EXCEPT;
  }
  /* asynchronous error ? */
  if( so->so_error)
    rflags |= NG_SIOSEL_ERROR;

  return( rflags);
}

