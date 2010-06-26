/*****************************************************************************
 * $Id: eth_out.c,v 1.3 2001/11/20 14:36:07 rf Exp $
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
 * ngEtherAddrBcast
 * ngEtherOutput()
 *----------------------------------------------------------------------------
 * 08/09/98 - Regis Feneon
 * 27/10/98 -
 * 29/12/98 -
 *  minor changes in arp global variables
 * 08/02/2000 -
 *  added NG_BUFF_MCAST test and multicast address generation
 * 30/11/2000 -
 *  added loopback of local destination packets
 * 20/11/2001 -
 *  sizeof(NGetherhdr) replaced with ETHER_HDR_LEN
 *  added const modifier to ethernet addresses
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>
#include <ngip/ethernet.h>

/* ethernet broadcast address */
const NGubyte ngEtherAddrBcast[6] = { 0xff,0xff,0xff,0xff,0xff,0xff };
const NGubyte ngEtherAddrZero[6] = { 0x00,0x00,0x00,0x00,0x00,0x00 };

/*****************************************************************************
 * ngEtherOutput()
 *****************************************************************************
 * Send a message to the ethernet layer
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 *  bufp        message to send
 *  in_addr     destination IP address
 * Return value: 0 if OK or error code
 */

int ngEtherOutput( NGifnet *netp, NGbuf *bufp, NGuint in_addr)
{
    int err, ictl;
    NGetherhdr *ehdrp;
    NGarpent *arpentp;

    /* the interface must be up and running */
    if( (netp->if_flags & (NG_IFF_UP|NG_IFF_RUNNING)) != (NG_IFF_UP|NG_IFF_RUNNING)) {
        err = NG_ENETDOWN;
        goto out_err;
    }

    /* test if the message has the ethernet header */
    if( !(bufp->buf_flags & NG_BUFF_RAW)) {

        /* destination = local address ? */
        if( in_addr == netp->if_addr) {
          /* just loopback the buffer */
          ngIpLoopback( netp, bufp);
          return( NG_EOK);
        }

        /* ethernet header pointer */
        ehdrp = (NGetherhdr *) (bufp->buf_datap - ETHER_HDR_LEN);

        /* this is an IP datagram */
        /* we have to find the destination physical address */
        /* ngArpResolve() in-line: */

        /* test if broadcast */
        if( bufp->buf_flags & NG_BUFF_BCAST) {
            /* dest = broadcast address */
            ngMemCpy( ehdrp->ether_dhost, ngEtherAddrBcast, sizeof( ehdrp->ether_dhost));
#ifdef XXX
            /* if broadcasting on a simplex interface, loopback a copy */
            if( netp->if_flags & NG_IFF_SIMPLEX) {
              NGbuf *tbufp;
              tbufp = ngBufCopy( bufp);
              if( tbufp != NULL) {
                netp->if_opackets--;
                netp->if_omcasts--;
                netp->if_obyes -= bufp->buf_datalen;
                ngIpLoopback( netp, bufp);
              }
            }
#endif
        }
        /* test if multicast */
        else if( bufp->buf_flags & NG_BUFF_MCAST) {
            /* construct ethernet multicast address */
            ehdrp->ether_dhost[0] = 0x01;
            ehdrp->ether_dhost[1] = 0x00;
            ehdrp->ether_dhost[2] = 0x5e;
            ehdrp->ether_dhost[3] = ((NGubyte *) &in_addr)[1] & 0x7f;
            ehdrp->ether_dhost[4] = ((NGubyte *) &in_addr)[2];
            ehdrp->ether_dhost[5] = ((NGubyte *) &in_addr)[3];
        }
        /* find or create an arp entry for this address */
        else if( (arpentp = ngArpLookUp( in_addr, 1)) != NULL) {
            if( arpentp->ae_flags & NG_ARPF_REJECT) {
                /* host is down... */
                err = NG_EHOSTDOWN;
                goto out_err;
            }
            if( arpentp->ae_flags & NG_ARPF_INCOMPLETE) {
                /* havent the address yet */
                if( arpentp->ae_holdmsg) {
                    /* free previous held message */
                    ngBufOutputFree( arpentp->ae_holdmsg);
                }
                /* hold the current message */
                arpentp->ae_holdmsg = bufp;

                if( arpentp->ae_asked <= 0) {
                /* send the first request */
                    arpentp->ae_expire = ngArp_wait;
                    arpentp->ae_asked = ngArp_retry;
                    ngArpRequest( netp, in_addr);
                }
                return( NG_EOK);
            }
            /* write dest address */
            ngMemCpy( ehdrp->ether_dhost, arpentp->ae_phaddr, sizeof( ehdrp->ether_dhost));
        }
        else {
            /* not enouth arp entries... */
            err = NG_ENOBUFS;
            goto out_err;
        }

        /* datagram type */
        ehdrp->ether_type = ngConstHTONS( ETHERTYPE_IP);
        /* adjust pointer and size of data */
        bufp->buf_datap -= ETHER_HDR_LEN;
        bufp->buf_datalen += ETHER_HDR_LEN;
    }
    else {
        /* raw mode */
        /* just write source address of datagram */
        ehdrp = (NGetherhdr *) bufp->buf_datap;
    }
    /* source (interface) address */
    ngMemCpy( ehdrp->ether_shost, NG_ETHIF_DATA( netp, eif_addr),
              sizeof( ehdrp->ether_shost));

    /* minimum size */
    if( bufp->buf_datalen < (ETHERMIN + ETHER_HDR_LEN))
        bufp->buf_datalen = ETHERMIN + ETHER_HDR_LEN;

    /* queue buffer in driver output queue */
    /* ...before test if queue is full */
    ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE);
    if( NG_QUEUE_FULL( &netp->if_outq)) {
        netp->if_oqdrops++;
        ngOSIntrCtl( ictl);
        /* forget the datagram */
        ngBufOutputFree( bufp);
        return( NG_ENOBUFS);
    }
    /* else queue the message */
    NG_QUEUE_IN( &netp->if_outq, bufp);
    ngOSIntrCtl( ictl);

    /* call driver start function */
    (netp->if_start_f)( netp);
    return( NG_EOK);

out_err:
    /* free buffer and returns */
    ngBufOutputFree( bufp);
    return( err);

}

