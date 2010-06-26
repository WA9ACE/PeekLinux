/*****************************************************************************
 * $Id: arp_look.c,v 1.2 2001/04/02 14:31:40 rf Exp $
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
 * ngArpLookup()
 *----------------------------------------------------------------------------
 * 17/09/98 - Regis Feneon
 * 27/10/98 -
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>
#include <ngip/ethernet.h>
#include <ngip/arp.h>

/*****************************************************************************
 * ngArpLookup()
 *****************************************************************************
 * Find or create an ARP entry
 * !! must be called from a protected section !!
 *****************************************************************************
 * Parameters:
 *  in_addr      ip address
 *  create       0=dont create, 1=create a empty entry if no current one
 * Return value: pointer to the ARP entry or NULL if not found
 */

NGarpent *ngArpLookUp( NGuint in_addr, int create)
{
    int i;
    NGarpent *arpentp;

    arpentp = NULL;
    /* find a entry that matches the ip address */
    for( i=0; i<ngArp_max; i++) {
        if( (ngArp_table[i].ae_flags & NG_ARPF_INUSE) &&
          (ngArp_table[i].ae_inaddr == in_addr)) {
            arpentp = &ngArp_table[i];
            break;
        }
    }
    if( (arpentp == NULL) && create) {
        /* find an empty entry */
        for( i=0; i<ngArp_max; i++) {
            if( !(ngArp_table[i].ae_flags & NG_ARPF_INUSE)) {
                arpentp = &ngArp_table[i];
                arpentp->ae_flags = NG_ARPF_INUSE|NG_ARPF_INCOMPLETE;
                arpentp->ae_expire = 0;
                arpentp->ae_asked = 0;
                arpentp->ae_inaddr = in_addr;
                arpentp->ae_holdmsg = NULL;
                break;
            }
        }
    }

    return( arpentp);
}

