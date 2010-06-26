/**
 * @file    rnet_rt_ng_input.c
 *
 * Riviera RNET - NexGenOS/NexGenIP event processing
 *
 * @author  Regis Feneon
 * @version 0.1
 */

/*
 * History:
 *
 *  Date        Author        Modification
 *  --------------------------------------------------
 *  3/14/2002   Regis Feneon  Riviera version
 *
 */

/*****************************************************************************
 * $Id: rnet_rt_ng_input.c,v 1.1.1.1 2002/04/03 17:12:41 rf Exp $
 * $Name: ti_20021030 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Polling Mode Incoming Messages Processing Routine
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
 * ngYield()
 *----------------------------------------------------------------------------
 * 11/09/98 - Regis Feneon
 * 08/02/2000 -
 *  increment if_noproto if no protocol found for input buffer
 * 23/06/2000 -
 *  added while() loop for dequeuing packets
 *  added polling routines processing
 * 19/07/2000 -
 *  added device events processing
 *  removed user polling stuff (now included in general polling routines)
 *  removed rtos version of ngYield()
 * 24/11/2000 -
 *  ngProtoList changed
 *  added global ngYield_lasttime initialized by ngInit()
 * 04/03/2001 -
 *  use global ngOSData
 *****************************************************************************/

#include "rnet_rt_i.h"

/*****************************************************************************
 * rnet_ngip_input()
 *****************************************************************************
 * Network input processing
 *****************************************************************************
 * Parameters:
 *  bufp      pointer to network buffer
 * Return value: none
 */

void rnet_rt_ngip_input( NGbuf *bufp)
{
  int i;

  /* get protocol number */
  i = bufp->buf_flags & NG_BUFF_PROTO;
  if( (i < NG_PROTO_MAX) &&
      (ngProtoList[i] != NULL) ) {
    /* call message protocol input function */
    ngProtoList[i]->pr_input_f( bufp);
  }
  else {
    /* no protocol found, free the buffer */
    ((NGifnet *) bufp->buf_ifnetp)->if_noproto++;
    NG_QUEUE_IN( &ngBuf_freeq, bufp);
  }
}

