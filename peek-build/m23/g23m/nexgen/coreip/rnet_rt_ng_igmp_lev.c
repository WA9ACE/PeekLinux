/*****************************************************************************
 * $Id: igmp_lev.c,v 1.2 2001/04/02 14:31:40 rf Exp $
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
 * ngIgmpLeaveGroup()
 *----------------------------------------------------------------------------
 * 17/11/2000 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngip/igmp.h>

#ifdef NG_IGMP_SUPPORTED

/*****************************************************************************
 * ngIgmpLeaveGroup()
 *****************************************************************************
 * Release a group structure
 *****************************************************************************
 * Parameters:
 *  inmp    pointer to group structure
 * Return value: none
 */

void ngIgmpLeaveGroup( NGinmulti *inmp)
{
  NGinmulti *tinmp, *tprevinmp;

  /* detach from interface */
  tprevinmp = NULL;
  tinmp = inmp->inm_ifnetp->if_multiaddrs;
  while( tinmp) {
    if( tinmp == inmp) break;
    tprevinmp = tinmp;
    tinmp = tinmp->inm_next;
  }
  if( tinmp == NULL) return;
  if( tprevinmp == NULL) {
    inmp->inm_ifnetp->if_multiaddrs = inmp->inm_next;
  }
  else {
    tprevinmp->inm_next = inmp->inm_next;
  }
  /* inform interface */
  (void)(inmp->inm_ifnetp->if_cntl_f)( inmp->inm_ifnetp, NG_CNTL_SET,
                                 NG_IFO_DELMULTI, &inmp->inm_addr);

  /* release structure */
  NG_MEMBLK_FREE( &ngIp_multi_free, inmp);

  return;
}

#endif

