/**
 * @file    rnet_rt_ng_start.c
 *
 * Riviera RNET - NexGenOS/NexGenIP Global Initialization
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
 * $Id: rnet_rt_ng_start.c,v 1.2 2002/04/30 12:44:46 rf Exp $
 * $Name: ti_20021030 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * NexGenOS Global Initialization
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
 * ngInit()
 *----------------------------------------------------------------------------
 * 21/10/98 - Regis Feneon
 * 27/10/98 -
 *  Added call to ngOSInit
 * 06/12/98 -
 *  Added new config table support
 * 30/12/98 -
 *  New configuration system
 * 12/01/99 -
 *  change in interface initialization - ngIfAdd is no more called from here
 * 30/06/99 -
 *  driver init function suppressed - now interface initialisation is done
 *  in ngInit function
 * 12/08/99 -
 *  version and copyright strings added
 * 15/09/99 -
 *  NG_IF_ADD: call driver init function if set
 * 11/10/99 -
 *  added device io interface options
 * 05/11/99 -
 *  added NG_BUFO_ALLOC_BUF_F option
 * 08/11/99 -
 *  ngDevioData moved here
 * 20/03/2000 -
 *  updated version string to 1.1A
 * 30/05/2000 -
 *  added error codes
 *  now network interfaces are initialized here even in rtos mode
 * 09/06/2000 -
 *  added initialization of static data
 * 19/07/2000 -
 *  v1.2 - added new device options and device task creation
 *  now network interfaces, buffers and socket are optionals
 *  added initialization of if_buflist for network interfaces
 * 22/08/2000 -
 *  initialization of new NGdev structure
 * 19/09/2000 -
 *  added test and error reporting for device and interface opening
 * 03/10/2000 -
 *  added buffer deallocation options
 * 05/10/2000 -
 *  initialization of polling list
 * 03/11/2000 -
 *  added initialization of if_setmulti_f
 * 24/11/2000 -
 *  now ngProtoList is an array indexed with the protocol number
 *  added ngYield_lasttime initialization
 *  netp->if_mtu = min( ngBufDataMax, netd->netd_mtu)
 * 18/01/2001 -
 *  initialize and lock global mutex before parsing the config table
 *  now unlock mutex correctly when driver errors
 *  removed calls to ngDebuGSetLevel() and ngDebugSetModule()
 * 08/02/2001 -
 *  initialize dev_iflags & dev_oflags to zero
 * 14/02/2001 -
 *  added initialization of ngSock_selnsleep and ngSock_selsem
 * 04/03/2001 -
 *  global data in ngOSData, added support for modules
 *  removed so_handle & dcb_handle
 * 29/05/2001 -
 *  UNLOCK before initializing modules
 *****************************************************************************/

#include "rnet_rt_i.h"

/*****************************************************************************
 * rnet_ngip_start()
 *****************************************************************************
 * Open network interfaces with NG_IFT_AUTOSTART flag
 *****************************************************************************
 * Parameters:
 *  none
 * Return value: error code
 */

int rnet_rt_ngip_start( void)
{
  NGifnet *netp;

  /* start network drivers now */
  netp = ngIfList;
  while( netp) {
    if( netp->if_type & NG_IFT_AUTOSTART) {
      netp->if_type &= ~NG_IFT_AUTOSTART;
      if( (netp->if_open_f)( netp) != NG_EOK) {
        RNET_RT_SEND_TRACE("RNET_RT: ngip_start: cannot open interface",RV_TRACE_LEVEL_ERROR);
        return( NG_EINIT_IFOPEN);
      }
    }
    netp = netp->if_next;
  }

  return( NG_EOK);
}

