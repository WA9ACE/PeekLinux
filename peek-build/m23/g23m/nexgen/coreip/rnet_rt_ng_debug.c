/**
 * @file    rnet_rt_ng_debug.c
 *
 * Riviera RNET - NexGenIP Debug and Trace Module
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
 * $Id: rnet_rt_ng_debug.c,v 1.3 2002/10/30 15:30:32 rf Exp $
 * $Name: ti_20021030 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Debug and Trace Module
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
 * ngDebug()
 *----------------------------------------------------------------------------
 * 03/12/98 - Regis Feneon
 * 19/01/99 -
 *  added ngDebugSetLevel and ngDebugSetModule
 * 18/10/99 -
 *  added FTP, Telnet and Web strings
 * 25/10/00 - Adrien Felon
 *  added NAT string
 * 11/02/2000 - Regis Feneon
 *  new proto for ngVPrintf, now uses macros NG_VA_XXX
 * 30/05/2000 -
 *  removed calls to ngPrintf()/ngVPrintf()
 *  now uses ngRawPrintf() and ngStdOutchar() because of locking in rtos mode
 *  added locks in ngDebugSetLevel() and ngDebugSetModule()
 * 08/09/2000 -
 *  corrected size of tmp buffer (!)
 * 11/09/2000 - Adrien Felon
 *  some variables renamed + no more static (for dbg shell cmd in shc_dbg.c)
 *  RF: routines ngDebugSetLevel() and ngDebugSetModule() moved to
 *  own source file
 * 27/03/2001 - Regis Feneon
 *  support for resolver
 *****************************************************************************/

#include "rnet_rt_i.h"
#include "rvf_api.h"

#include <ngos.h>
#include <ngos/debug.h>

#ifdef RNET_RT_NGIP_DEBUG_ENABLE

int ngDebugLevel;

const char * const ngDebugModStr[] = {
  "????",
  "CORE",
  " DRV",
  " ETH",
  " ARP",
  " PPP",
  "  IP",
  "ICMP",
  " UDP",
  " TCP",
  "RWIP",
  "SOCK",
  " APP",
  "BOOT",
  "DHCP",
  "TFTP",
  "MAIL",
  " FTP",
  "FTPS",
  "FTPC",
  "TLNS",
  "TLNC",
  " WEB",
  " NAT",
  "PPOE",
  "SNMP",
  "RSLV",
  "IMAP",
};
static const char * const sub_str[] = {
  "-????",
  "-INIT",
  "-EXIT",
  "-INP ",
  "-OUT ",
  "-TIME",
  "-USER",
};
int ngDebugModOnOff[sizeof( ngDebugModStr)/sizeof( ngDebugModStr[0])];

/*****************************************************************************
 * ngDebug()
 *****************************************************************************
 * Write debug output from a module
 * use porting routine ngStdOutChar()
 *****************************************************************************
 * Parameters:
 *  module  module number
 *  submod  subfunction of module
 *  level   level of debugging
 *  format  format string (see ngRawPrintf())
 * Return value: none
 */

void ngDebug(int module, int submod, int level, const char *str,...)
{
  NGva_list ap;
  int n;

  if( level < ngDebugLevel) return;

  if( (unsigned)module >= sizeof( ngDebugModStr)/sizeof( ngDebugModStr[0])) module = 0;
  if( (unsigned)submod >= sizeof( sub_str)/sizeof( sub_str[0])) submod = 0;

  if( ngDebugModOnOff[module]) {

    ngMemCpy( &rnet_rt_env_ctrl_blk_p->buf_debug[0], "RNET_RT: ", 9);
    ngMemCpy( &rnet_rt_env_ctrl_blk_p->buf_debug[9], ngDebugModStr[module], 4);
    ngMemCpy( &rnet_rt_env_ctrl_blk_p->buf_debug[13], sub_str[submod], 5);

    NG_VA_START( ap, str);
    n = ngVSPrintf( &rnet_rt_env_ctrl_blk_p->buf_debug[18], str, ap);
    NG_VA_END( ap);

    /* Make sure the string is not too long for the frame's trace functions,
     * if necessary, by tracing it in two portions. [ni 2004-01-06] */
#define TRACE_CUTOFF 64
    if (n + 18 > TRACE_CUTOFF)
    {
      char *contents ;          /* Pointer to trace contents. */
      char tmpbuf[TRACE_CUTOFF+1] ; /* Buffer second half of contents. */

      contents = rnet_rt_env_ctrl_blk_p->buf_debug ;
      strncpy(tmpbuf, contents + TRACE_CUTOFF, TRACE_CUTOFF) ;
      tmpbuf[TRACE_CUTOFF] = '\0' ;
      strcpy(contents + TRACE_CUTOFF, "...") ;
      rvf_send_trace( contents, (UINT8) strlen(contents),
                      NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, RNET_RT_USE_ID);
      strcpy(contents, "...") ;
      strcat(contents, tmpbuf) ;
      rvf_send_trace( contents, (UINT8) strlen(contents),
                      NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, RNET_RT_USE_ID);
    }
    else
    {
      rvf_send_trace( rnet_rt_env_ctrl_blk_p->buf_debug, (UINT8) (18 + n),
                      NULL_PARAM, RV_TRACE_LEVEL_DEBUG_LOW, RNET_RT_USE_ID);
    }
  }
}

#else

void ngDebug( int module, int submod, int level, const char *str,...)
{
  ((void)module);((void)submod);((void)level);((void)str);
  return;
}

#endif

