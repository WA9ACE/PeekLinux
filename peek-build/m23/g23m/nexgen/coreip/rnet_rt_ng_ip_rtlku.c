/*****************************************************************************
 * $Id: ip_rtlku.c,v 1.3 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * IP Internal Routing functions
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
 * ngIpRouteLookup()
 *----------------------------------------------------------------------------
 * 05/11/98 - Regis Feneon
 * 30/12/98 -
 *  minor changes in ip global variables
 * 12/01/98 -
 *  test for limited broadcast added
 * 11/02/99 -
 *  name changed from ngRouteLookup() to ngIpRouteLookup() to distinguish
 *  from user functions ngRouteXXX()
 * 09/02/2000 -
 *  added test for multicast address
 * 12/09/2000 -
 *  added scan for broadcast and multicast interfaces,
 *   now select the first broadcast or multicast interface,
 *   not always the first interface in the list
 *  added setting of subnetmask in direct route selection
 *  added test for point-to-point interface
 * 05/10/2000 -
 *  never select uninitialized interfaces for direct routes
 * 18/02/2001 -
 *  added dontroute flag
 *****************************************************************************/

#include <ngip.h>



/*****************************************************************************
 * ngIpRouteLookup()
 *****************************************************************************
 * Find the interface and gateway to reach a host
 *****************************************************************************
 * Parameters:
 *  addr        host address
 *  g_addr      filled with gateway address
 * Return value: network interface
 */

NGifnet *ngIpRouteLookup( NGuint addr, NGuint *g_addr, int dontroute)
{
  int i;
  NGuint gateway, gatemask;
  NGifnet *netp, *tnetp;

  /* test for limited broadcast */
  if( addr == NG_INADDR_BROADCAST) {
    *g_addr = addr;
    /* first broadcast interface */
    netp = ngIfList;
    while( netp != NULL) {
        if( netp->if_flags & NG_IFF_BROADCAST) break;
        netp = netp->if_next;
    }
    return( netp);
  }
  /* test for multicast address */
  if( NG_IN_CLASSD( addr)) {
    *g_addr = addr;
    /* first multicast interface */
    netp = ngIfList;
    while( netp != NULL) {
        if( netp->if_flags & NG_IFF_MULTICAST) break;
        netp = netp->if_next;
    }
    return( netp);
  }

  /* select the best route */
  /* direct routes first */
  gateway = 0;
  gatemask = 0;
  netp = NULL;
  tnetp = ngIfList;
  while( tnetp != NULL) {
    if( tnetp->if_flags & NG_IFF_POINTOPOINT) {
      /* test for destination address if point-to-point */
      if( addr == tnetp->if_dstaddr) {
        *g_addr = addr;
        return( tnetp);
      }
    }
    else if( (tnetp->if_addr) && /* 05/10/2000 */
             ((addr & tnetp->if_subnetmask) == tnetp->if_subnet) &&
             ((gatemask == 0) || (tnetp->if_subnetmask > gatemask)) ) {
      gateway = addr;
      gatemask = tnetp->if_subnetmask;
      netp = tnetp;
    }
    tnetp = tnetp->if_next;
  }

  /* return now if routing disabled */
  if( dontroute) {
    *g_addr = gateway;
    return( netp);
  }

  /* other routes */
  for (i=0; i<ngIp_route_max; i++) {
    if (
        (addr & ngIp_route_table[i].rt_subnetmask) ==
        ngIp_route_table[i].rt_addr
    ) {/*20031209 add second condition here to make  sure netp won't be covered by NULL*/
      if( ngIp_route_table[i].rt_subnetmask > gatemask/* && ngIp_route_table[i].rt_ifnetp*/ ) {
        gateway = ngIp_route_table[i].rt_gateway ?
          ngIp_route_table[i].rt_gateway : addr;
        gatemask = ngIp_route_table[i].rt_subnetmask;
        netp = ngIp_route_table[i].rt_ifnetp;
      }
    }
  }

  if( gateway == 0) {
    /* default route */
    if( !ngIp_route_default.rt_gateway) {
      return( NULL);
    }
    gateway = ngIp_route_default.rt_gateway;
    netp = ngIp_route_default.rt_ifnetp;
  }

  *g_addr = gateway;
  return( netp);
}

