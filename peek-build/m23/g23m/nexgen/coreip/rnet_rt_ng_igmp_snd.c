/*****************************************************************************
 * $Id: igmp_snd.c,v 1.3 2001/11/20 14:36:07 rf Exp $
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
 * ngIgmpSendReport()
 *----------------------------------------------------------------------------
 * 17/11/2000 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngip/ip.h>
#include <ngip/igmp.h>

#ifdef NG_IGMP_SUPPORTED

/*****************************************************************************
 * ngIgmpSendReport()
 *****************************************************************************
 * Send a IGMP report message for a group
 *****************************************************************************
 * Parameters:
 *  inmp    pointer to multicast group structure
 * Return value: none
 */

void ngIgmpSendReport( NGinmulti *inmp)
{
  NGbuf *bufp;
  struct igmpip {
    NGiphdr ip;
    NGigmp igmp;
  } *hdr;

  /* allocate one buffer */
  ngBufAlloc( bufp);
  if( bufp == NULL) return;

  /* prepare data */
  hdr = (struct igmpip *) (((NGubyte *) bufp) + ngBufDataOffset);
  bufp->buf_datap = (NGubyte *) hdr;
  bufp->buf_datalen = sizeof( struct igmpip);

  /* IP header */
  hdr->ip.ip_hlv = 5;
  hdr->ip.ip_tos = 0;
  hdr->ip.ip_len = sizeof( struct igmpip);
  hdr->ip.ip_off = 0;
  hdr->ip.ip_ttl = 1;
  hdr->ip.ip_p = IPPROTO_IGMP;
  hdr->ip.ip_src = NG_INADDR_ANY;
  hdr->ip.ip_dst = inmp->inm_addr;
  /* IGMP header */
  hdr->igmp.igmp_type = IGMP_HOST_MEMBERSHIP_REPORT;
  hdr->igmp.igmp_code = 0;
  hdr->igmp.igmp_group = inmp->inm_addr;
  hdr->igmp.igmp_cksum = 0;
  hdr->igmp.igmp_cksum = ngIpCksum( &hdr->igmp, sizeof( NGigmp), NULL, 0);

  /* send message using group interface */
  bufp->buf_ifnetp = inmp->inm_ifnetp;
  bufp->buf_flags |= NG_BUFF_SETIF;
  ngIpOutput( bufp, 0);

#ifdef NG_IPSTATS_SUPPORTED
  ngIgmpStat.igps_oreports++;
#endif

  return;
}

#endif

