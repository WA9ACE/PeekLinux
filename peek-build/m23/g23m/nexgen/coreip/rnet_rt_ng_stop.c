/**
 * @file    rnet_rt_ng_stop.c
 *
 * Riviera RNET - NexGenOS/NexGenIP standard routines
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
 * $Id: rnet_rt_ng_stop.c,v 1.1.1.1 2002/04/03 17:12:41 rf Exp $
 * $Name: ti_20021030 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Standart routines
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
 * ngExit()
 *----------------------------------------------------------------------------
 * 21/10/98 - Regis Feneon
 * 27/10/98 -
 *  Added call to ngOSExit
 * 26/10/99 -
 *  removed call to ngIfClose(), now call directly if_close_f
 * 03/09/2000 -
 *  added calls to buffer deallocation routines
 * 04/03/2001 -
 *  support for modules
 * 07/05/2001 -
 *  removed calls to LOCK/UNLOCK for calling modules exit function
 *****************************************************************************/

#include "rnet_rt_i.h"

/*****************************************************************************
 * rnet_ngip_stop()
 *****************************************************************************
 * Close all network interfaces
 *****************************************************************************
 * Parameters:
 *  none
 * Return value: none
 */

void rnet_rt_ngip_stop( void)
{
  NGifnet *netp;

  /* shutdown interfaces */
  netp = ngIfList;
  while( netp) {
    (void)(netp->if_close_f)( netp);
    netp = netp->if_next;
  }
}

