/*****************************************************************************
 * $Id: arp_req.c,v 1.3 2001/11/20 14:36:07 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Address Resolution Protocol Functions
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
 * ngArpRequest()
 *----------------------------------------------------------------------------
 * 17/09/98 - Regis Feneon
 * 27/10/98 -
 * 23/05/99 -
 *  debug output added
 * 20/11/2001 -
 *  sizeof(NGetherhdr) replaced with ETHER_HDR_LEN
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>
#include <ngip/ethernet.h>
#include <ngip/arp.h>

/*****************************************************************************
 * ngArpRequest()
 *****************************************************************************
 * Construct and send an ARP request
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 *  in_addr     requested address
 * Return value: none
 */

void ngArpRequest( NGifnet *netp, NGuint in_addr)
{
    NGbuf *bufp;
    NGetherarp *earpp;

    /* allocate a message buffer */
    ngBufAlloc( bufp);
    if( bufp == NULL) return; /* not enougth buffers... */

    /* construction of arp message */
    bufp->buf_datap = ((NGubyte *) bufp) + ngBufDataOffset - ETHER_HDR_LEN;
    bufp->buf_datalen = ETHER_HDR_LEN + sizeof(NGetherarp);
    /* ethernet header */
    /* destination = broadcast */
    ngMemCpy( ((NGetherhdr *) bufp->buf_datap)->ether_dhost, ngEtherAddrBcast,
                sizeof( ngEtherAddrBcast));
    /* source is filled by ngEtherOutput() */
    /* type ARP */
    ((NGetherhdr *) bufp->buf_datap)->ether_type = ngConstHTONS( ETHERTYPE_ARP);

    /*lint -e572 (Warning -- Excessive shift value) */
    /* arp message */
    earpp = (NGetherarp *) (((NGubyte *) bufp) + ngBufDataOffset);
    earpp->arp_hrd = ngConstHTONS( ARPHRD_ETHER);
    earpp->arp_pro = ngConstHTONS( ETHERTYPE_IP);
    earpp->arp_hln = sizeof( earpp->arp_sha);
    earpp->arp_pln = sizeof( earpp->arp_spa);
    earpp->arp_op = ngConstHTONS( ARPOP_REQUEST);
    /*lint +e572 (Warning -- Excessive shift value) */
    /* sender address */
    ngMemCpy( earpp->arp_sha, NG_ETHIF_DATA( netp, eif_addr), sizeof( earpp->arp_sha));
    ngMemCpy( earpp->arp_spa, &netp->if_addr, sizeof( earpp->arp_spa));
    /* target */
    ngMemSet( earpp->arp_tha, 0, sizeof( earpp->arp_tha));
    ngMemCpy( earpp->arp_tpa, &in_addr, sizeof( earpp->arp_tpa));

#ifdef NG_DEBUG
    ngDebug( NG_DBG_ARP, NG_DBG_OUTPUT, 0, "query %E %I > %E %I",
     earpp->arp_sha, netp->if_addr, earpp->arp_tha, in_addr);
#endif

    /* send the message */
    bufp->buf_flags |= NG_BUFF_RAW|NG_BUFF_BCAST;
    (void)(netp->if_output_f)( netp, bufp, 0);
}

