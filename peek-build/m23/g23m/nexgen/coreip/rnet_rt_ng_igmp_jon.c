/*****************************************************************************
 * $Id: igmp_jon.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet Group Management Protocol Functions
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
 * ngIgmpJoinGroup()
 *----------------------------------------------------------------------------
 * 17/11/2000 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngip/igmp.h>

#ifdef NG_IGMP_SUPPORTED

/*****************************************************************************
 * ngIgmpJoinGroup()
 *****************************************************************************
 * Allocate and attach a new group structure to an interface
 *****************************************************************************
 * Parameters:
 *  netp    pointer to interface
 *  addr    group address
 * Return value: pointer to allocated structure or NULL
 */

NGinmulti *ngIgmpJoinGroup( NGifnet *netp, NGuint addr)
{
  NGinmulti *inmp;

  /* try to allocate a new structure */
  NG_MEMBLK_ALLOC( &ngIp_multi_free, inmp);
  if( inmp == NULL) return( NULL);

  /* initialize structure */
  inmp->inm_ifnetp = netp;
  inmp->inm_addr = addr;
  inmp->inm_refcount = 0;
  /* send report later */
  inmp->inm_timer = (((ngRandom( &ngIp_random) & 0xff) *
                        IGMP_MAX_HOST_REPORT_DELAY*NG_TIMERHZ)>>8) + 1;
  ngIp_multi_timers = 1;

  /* attach to interface */
  inmp->inm_next = netp->if_multiaddrs;
  netp->if_multiaddrs = inmp;
  (void)(netp->if_cntl_f)( netp, NG_CNTL_SET, NG_IFO_ADDMULTI, &addr);

  /* and send one report now */
  ngIgmpSendReport( inmp);

  return( inmp);
}

#endif

