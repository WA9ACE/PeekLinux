/*****************************************************************************
 * $Id: ip_rtupd.c,v 1.2 2001/04/02 14:31:40 rf Exp $
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
 * ngIpRouteUpdate()
 *----------------------------------------------------------------------------
 * 11/02/99 - Regis Feneon
 * 25/04/99 -
 *  function corrected
 * 10/02/00 - Adrien Felon
 *  added test for PPP protocol interface (accepting either local or
 *  remote address as gateway)
 * 04/10/2000 - Regis Feneon
 *  skip uninitialised interfaces
 *****************************************************************************/

#include <ngip.h>

/*****************************************************************************
 * ngIpRouteUpdate()
 *****************************************************************************
 * Update the interface pointers of routing table entries
 *****************************************************************************
 * Parameters: none
 * Return value: none
 */

void ngIpRouteUpdate( void)
{
  int i;
  NGifnet *netp;

  /* update default route */
  if( ngIp_route_default.rt_gateway != NG_INADDR_ANY) {
    ngIp_route_default.rt_ifnetp = NULL;
    netp = ngIfList;
    while( netp) {
      if( netp->if_addr != NG_INADDR_ANY) { /* 04/10/2000 */
        if (netp->if_flags & NG_IFF_POINTOPOINT) {
          if (
            (ngIp_route_default.rt_gateway == netp->if_addr) ||
            (ngIp_route_default.rt_gateway == netp->if_dstaddr)
          ) {
            ngIp_route_default.rt_ifnetp = netp;
            break;
          }
        }
        else if( (ngIp_route_default.rt_gateway & netp->if_subnetmask)
                 == netp->if_subnet) {
          ngIp_route_default.rt_ifnetp = netp;
          break;
        }
      }
      netp = netp->if_next;
    }
  }

  /* update other routes */
  for( i=0; i<ngIp_route_max; i++) {
    if( ngIp_route_table[i].rt_gateway == NG_INADDR_ANY) continue;
    ngIp_route_table[i].rt_ifnetp = NULL;
    netp = ngIfList;
    while( netp) {
      if( netp->if_addr != NG_INADDR_ANY) { /* 04/10/2000 */
        if (netp->if_flags & NG_IFF_POINTOPOINT) {
          if (
              (ngIp_route_table[i].rt_gateway == netp->if_addr) ||
              (ngIp_route_table[i].rt_gateway == netp->if_dstaddr)
          ) {
            ngIp_route_table[i].rt_ifnetp = netp;
            break;
          }
        }
        else if( (ngIp_route_table[i].rt_gateway & netp->if_subnetmask)
                 == netp->if_subnet) {
          ngIp_route_table[i].rt_ifnetp = netp;
          break;
        }
      }
      netp = netp->if_next;
    }
  }
}

