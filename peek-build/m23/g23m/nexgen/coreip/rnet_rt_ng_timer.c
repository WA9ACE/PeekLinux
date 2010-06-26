/**
 * @file    rnet_rt_ng_timer.c
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
 *  28/10/2002   Regis Feneon  increment system clock for
 *                             implementation of ngOSClockGetTime()
 *
 */

/*****************************************************************************
 * $Id: rnet_rt_ng_timer.c,v 1.2 2002/10/30 15:30:32 rf Exp $
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
 * rnet_ngip_timer()
 *****************************************************************************
 * Protocols timer processing
 *****************************************************************************
 * Parameters:
 *  none
 * Return value: none
 */

void rnet_rt_ngip_timer( void)
{
  int i;

  /* increment system clock */
  rnet_rt_env_ctrl_blk_p->clock++;

  /* call the protocols timer function */
  for( i=0; i<NG_PROTO_MAX; i++) {
    if( (ngProtoList[i] != NULL) &&
      (ngProtoList[i]->pr_timer_f != NULL) ) {
      ngProtoList[i]->pr_timer_f();
    }
  }
}

/* returns system clock */
u_long ngOSClockGetTime( void)
{
  return( rnet_rt_env_ctrl_blk_p->clock);
}

