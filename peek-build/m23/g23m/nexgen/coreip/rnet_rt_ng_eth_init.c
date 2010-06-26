/*****************************************************************************
 * $Id: eth_init.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Generic Ethernet Interface Functions
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
 * ngEtherInit()
 *----------------------------------------------------------------------------
 * 06/11/2000 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>
#include <ngip/ethernet.h>

/*****************************************************************************
 * ngEtherInit()
 *****************************************************************************
 * Initialize ethernet interface structure
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 * Return value: 0
 */

int ngEtherInit( NGifnet *netp)
{
  int i;

  /* reset structure fields */
  NG_ETHIF_DATA( netp, eif_flags) = 0;
  ngMemSet( NG_ETHIF_DATA( netp, eif_addr), 0, 6);
  NG_ETHIF_DATA( netp, eif_irq) = 0;
  NG_ETHIF_DATA( netp, eif_iobase) = 0;
  NG_ETHIF_DATA( netp, eif_base) = NULL;
  NG_ETHIF_DATA( netp, eif_physbase) = 0;
  for( i=0; i<NG_ETHIF_BUFI_MAX; i++)
    NG_ETHIF_DATA( netp, eif_bufi[i]) = NULL;
  for( i=0; i<NG_ETHIF_BUFO_MAX; i++)
    NG_ETHIF_DATA( netp, eif_bufo[i]) = NULL;

  /* multicast filters */
  NG_ETHIF_DATA( netp, eif_allmultis) = 0;
#ifdef NG_IGMP_SUPPORTED
  /* join all-hosts group 224.0.0.1 */
  NG_ETHIF_DATA( netp, eif_multiaddrs)[0].eifm_refcount = 1;
  NG_ETHIF_DATA( netp, eif_multiaddrs)[0].eifm_addr[0] = 0x01;
  NG_ETHIF_DATA( netp, eif_multiaddrs)[0].eifm_addr[1] = 0x00;
  NG_ETHIF_DATA( netp, eif_multiaddrs)[0].eifm_addr[2] = 0x5e;
  NG_ETHIF_DATA( netp, eif_multiaddrs)[0].eifm_addr[3] = 0x00;
  NG_ETHIF_DATA( netp, eif_multiaddrs)[0].eifm_addr[4] = 0x00;
  NG_ETHIF_DATA( netp, eif_multiaddrs)[0].eifm_addr[5] = 0x01;
  for( i=1; i<NG_ETHIF_MULTIADDRS_MAX; i++)
#else
  for( i=0; i<NG_ETHIF_MULTIADDRS_MAX; i++)
#endif
    NG_ETHIF_DATA( netp, eif_multiaddrs)[i].eifm_refcount = 0;

  return( NG_EOK);
}

