/*****************************************************************************
 * $Id: arp.c,v 1.3 2001/11/20 14:36:07 rf Exp $
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
 * arpInit()
 * arpTimer()
 * arpInput()
 * ngProto_ARP
 *----------------------------------------------------------------------------
 * 22/09/98 - Regis Feneon
 * 27/10/98 -
 * 29/12/98 -
 *  added arpCntl()
 * 09/06/2000 -
 *  removed unused routine arpInit()
 * 20/07/2000 -
 *  added NG_ARPO_ADDENTRY and NG_ARPO_DELENTRY processing
 * 19/09/2000 -
 *  arpCntl() - added initializing of the arp table
 *  arpInit() - is back, reset global data
 * 20/11/2001 -
 *  sizeof(NGetherhdr) replaced with ETHER_HDR_LEN
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>
#include <ngip/ethernet.h>
#include <ngip/arp.h>

int ngArp_max; /* size of arp table */
NGarpent *ngArp_table; /* arp table */
int ngArp_retry; /* maximum number of retries */
u_int ngArp_wait; /* pause between two retries (1 sec) */
u_int ngArp_expire; /* expiration timer (30 sec) */
u_int ngArp_reject; /* reject timer (15 sec) */

/*****************************************************************************
 * arpInit()
 *****************************************************************************
 * Initialize ARP layer
 *****************************************************************************
 * Parameters: none
 * Return value: none
 */

static void arpInit( void)
{
    ngArp_max = 0;
    ngArp_table = NULL;
    ngArp_retry = 5;
    ngArp_wait = 1*NG_TIMERHZ;
    ngArp_expire = 30*NG_TIMERHZ;
    ngArp_reject = 15*NG_TIMERHZ;
}

/*****************************************************************************
 * arpCntl()
 *****************************************************************************
 * ARP Control Protocol Function
 *****************************************************************************
 * Parameters:
 *  cmd     NG_CNTL_SET or NG_CNTL_GET
 *  opt     option (see net_eth.h)
 *  arg     argument
 * Return value:
 *  0 or -1 (error)
 */

static int arpCntl( int cmd, int opt, void *arg)
{
    int i;
    NGuint inaddr;

    if( cmd == NG_CNTL_SET) switch( opt) {
    case NG_ARPO_MAX:
        if( ngArp_max) return( NG_EINVAL);
        ngArp_max = *((int *) arg);
        break;
    case NG_ARPO_TABLE:
        if( !ngArp_max) return( NG_EINVAL);
        ngArp_table = *((NGarpent **) arg);
        for( i=0; i<ngArp_max; i++) {
            /* clear flags */
            ngArp_table[i].ae_flags = 0;
        }
        break;
    case NG_ARPO_RETRY:
        ngArp_retry = *((int *) arg);
        break;
    case NG_ARPO_WAIT:
        ngArp_wait = *((u_int *) arg)*NG_TIMERHZ;
        break;
    case NG_ARPO_EXPIRE:
        ngArp_expire = *((u_int *) arg)*NG_TIMERHZ;
        break;
    case NG_ARPO_REJECT:
        ngArp_reject = *((u_int *) arg)*NG_TIMERHZ;
        break;
    case NG_ARPO_ADDENTRY:
    case NG_ARPO_DELENTRY:
        inaddr = ((NGarphost *) arg)->arp_inaddr;
        /* first delete the existing entry */
        for( i=0; i<ngArp_max; i++) {
            if( ngArp_table[i].ae_flags &&
                 (ngArp_table[i].ae_inaddr == inaddr) ) {
                 /* remove the entry */
                 ngArp_table[i].ae_flags = 0;
                 /* free pending buffer */
                 if( ngArp_table[i].ae_holdmsg) {
                    ngBufOutputFree( ngArp_table[i].ae_holdmsg);
                    ngArp_table[i].ae_holdmsg = NULL;
                 }
                 if( opt == NG_ARPO_DELENTRY) return( NG_EOK);
                 break;
            }
        }
        /* not found... */
        if( opt == NG_ARPO_DELENTRY) return( NG_ENOENT);
        /* add the new entry... */
        /* find a empty slot */
        for( i=0; i<ngArp_max; i++) {
            if( ngArp_table[i].ae_flags == 0) {
                /* set a static entry */
                ngArp_table[i].ae_flags = NG_ARPF_INUSE|NG_ARPF_STATIC;
                ngArp_table[i].ae_inaddr = inaddr;
                ngMemCpy( &ngArp_table[i].ae_phaddr[0],
                          &((NGarphost *) arg)->arp_phaddr[0],
                          sizeof( ngArp_table[i].ae_phaddr));
                return( NG_EOK);
            }
        }
        /* all entries are in use... */
        return( NG_ENOSPC);
    default:
        return( NG_EINVAL);
    }
    else if( cmd == NG_CNTL_GET) switch( opt) {
    case NG_ARPO_MAX:
        *((int *) arg) = ngArp_max;
        break;
    case NG_ARPO_TABLE:
        *((NGarpent **) arg) = ngArp_table;
        break;
    case NG_ARPO_RETRY:
        *((int *) arg) = ngArp_retry;
        break;
    case NG_ARPO_WAIT:
        *((u_int *) arg) = ngArp_wait/NG_TIMERHZ;
        break;
    case NG_ARPO_EXPIRE:
        *((u_int *) arg) = ngArp_expire/NG_TIMERHZ;
        break;
    case NG_ARPO_REJECT:
        *((u_int *) arg) = ngArp_reject/NG_TIMERHZ;
        break;
    default:
        return( NG_EINVAL);
    }
    else return( NG_EINVAL);
    return( NG_EOK);
}

/*****************************************************************************
 * arpTimer()
 *****************************************************************************
 * Update the ARP entries every 200ms
 *****************************************************************************
 * Parameters: none
 * Return value: none
 */

static void arpTimer( void)
{
    NGarpent *ap;
    int i;

    for( i=0, ap = ngArp_table; i<ngArp_max; i++, ap++) {
        if( (ap->ae_flags & (NG_ARPF_INUSE|NG_ARPF_STATIC)) == NG_ARPF_INUSE) {

            /* see if the timer has expired */
            if( --ap->ae_expire > 0) continue;

            if( (ap->ae_flags & NG_ARPF_REJECT) ||
             !(ap->ae_flags & NG_ARPF_INCOMPLETE)) {
                /* entry was completed or rejected -> remove */
                ap->ae_flags = 0;
#ifdef NG_DEBUG
                ngDebug( NG_DBG_ARP, NG_DBG_TIMER, 0, "Removing %I entry", ap->ae_inaddr);
#endif
            }
            else {
                /* incomplete entry, max retries reached ? */
                if( --ap->ae_asked > 0) {
                    /* no, send a new request */
                    ap->ae_expire = ngArp_wait;
                    ngArpRequest( ap->ae_holdmsg->buf_ifnetp, ap->ae_inaddr);
                }
                else {
                    /* reject the entry */
                    ap->ae_flags |= NG_ARPF_REJECT;
                    ap->ae_expire = ngArp_reject;
                    /* delete the held message */
                    ngBufOutputFree( ap->ae_holdmsg);
                    ap->ae_holdmsg = NULL;
#ifdef NG_DEBUG
                    ngDebug( NG_DBG_ARP, NG_DBG_TIMER, 0, "Rejecting host %I", ap->ae_inaddr);
#endif
                }
            }
        }
    }

}

/*****************************************************************************
 * arpInput()
 *****************************************************************************
 * Reception of an ARP message
 *****************************************************************************
 * Parameters:
 *  bufp   message buffer
 * Return value: none
 */

static void arpInput( NGbuf *bufp)
{
    NGifnet *netp;
    NGetherarp *earpp;
    NGuint in_saddr, in_taddr;
    NGarpent *arpentp;
    NGbuf *tmpbufp;

    earpp = (NGetherarp *) bufp->buf_datap;

    /* source and target IP address */
    ngMemCpy( &in_saddr, earpp->arp_spa, sizeof( NGuint));
    ngMemCpy( &in_taddr, earpp->arp_tpa, sizeof( NGuint));
#ifdef NG_DEBUG
    ngDebug( NG_DBG_ARP, NG_DBG_INPUT, 0, "op:%d src:%E %I dst:%E %I",
      ngNTOHS( earpp->arp_op), earpp->arp_sha, in_saddr, earpp->arp_tha, in_taddr);
#endif
    /* is the message valid ? */
    /*lint -e572 (Warning -- Excessive shift value) */
    if( (earpp->arp_hrd != ngConstHTONS( ARPHRD_ETHER)) ||
        (earpp->arp_pro != ngConstHTONS( ETHERTYPE_IP)) ||
        (earpp->arp_hln != sizeof( earpp->arp_sha)) ||
        (earpp->arp_pln != sizeof( earpp->arp_spa)) )
        goto bad;
    /*lint +e572 (Warning -- Excessive shift value) */

    netp = (NGifnet *) bufp->buf_ifnetp;

    /* if it's from us, ignore it */
    if( ngMemCmp( earpp->arp_sha, NG_ETHIF_DATA( netp, eif_addr),
                  sizeof( earpp->arp_sha)) == 0) {
        goto dropit;
    }

    /* ignore source broadcast address */
    if( ngMemCmp( earpp->arp_sha, ngEtherAddrBcast, sizeof( earpp->arp_sha)) == 0)
        goto bad;

    if( in_saddr != netp->if_addr) {

        /* add or update an entry for the sender in our arp table */
        if( (arpentp = ngArpLookUp( in_saddr, in_taddr == netp->if_addr)) != NULL) {
            ngMemCpy( arpentp->ae_phaddr, earpp->arp_sha, sizeof( earpp->arp_sha));

            if( !(arpentp->ae_flags & NG_ARPF_STATIC) ||
             (arpentp->ae_flags & NG_ARPF_INCOMPLETE))
                arpentp->ae_expire = ngArp_expire;
            arpentp->ae_flags &= ~(NG_ARPF_REJECT|NG_ARPF_INCOMPLETE);

            /* message pending ? */
            tmpbufp = arpentp->ae_holdmsg;
            arpentp->ae_holdmsg = NULL;

            if( tmpbufp) {
                /* call driver output function to send the pending message */
                (void)(netp->if_output_f)( netp, tmpbufp, in_saddr);
            }
        }

    }
    else {
        /* duplicate IP address... */
#ifdef NG_DEBUG
        ngDebug( NG_DBG_ARP, NG_DBG_INPUT, 1, "Duplicate address %I", in_saddr);
#endif
    }

    /* is the message for us ? */
    if( in_taddr != netp->if_addr) {
        goto dropit;
    }

    /* does it need a reply ? */
    /*lint -e572 (Warning -- Excessive shift value) */
    if( earpp->arp_op != ngConstHTONS( ARPOP_REQUEST))
        goto dropit;

    /* reply to the sender */
#ifdef NG_DEBUG
    ngDebug( NG_DBG_ARP, NG_DBG_INPUT, 0, "Send reply to %I", in_saddr);
#endif

    /* transform the request message to a reply message */
    earpp->arp_op = ngConstHTONS( ARPOP_REPLY);
    /*lint +e572 (Warning -- Excessive shift value) */

    /* target physical address */
    ngMemCpy( earpp->arp_tha, earpp->arp_sha, sizeof( earpp->arp_sha));
    /* target ip address */
    ngMemCpy( earpp->arp_tpa, earpp->arp_spa, sizeof( earpp->arp_spa));
    /* source physical address */
    ngMemCpy( earpp->arp_sha, NG_ETHIF_DATA( netp, eif_addr), sizeof( earpp->arp_sha));
    /* source ip address */
    ngMemCpy( earpp->arp_spa, &netp->if_addr, sizeof( earpp->arp_spa));

    /* ethernet header */
    bufp->buf_datap -= ETHER_HDR_LEN;
    bufp->buf_datalen += ETHER_HDR_LEN;
    /* this is a raw ethernet message */
    bufp->buf_flags |= NG_BUFF_RAW;
    /* dont broadcast the reply */
    bufp->buf_flags &= ~NG_BUFF_BCAST;
    /* destination */
    ngMemCpy( ((NGetherhdr *) bufp->buf_datap)->ether_dhost, earpp->arp_tha,
              sizeof( earpp->arp_tha));

    /* send the reply */
    (void)(netp->if_output_f)( netp, bufp, 0);

    return;

bad:
#ifdef NG_DEBUG
    ngDebug( NG_DBG_ARP, NG_DBG_INPUT, 1, "Invalid message");
#endif
dropit:
    /* free the message */
    ngBufFree( bufp);
}

/*****************************************************************************
 * ngProto_ARP
 *****************************************************************************
 * ARP Protocol Definition Structure
 */

const NGproto ngProto_ARP = {
    "ARP",
    0,
    0,
    NG_PROTO_ARP,
    arpInit,
    arpCntl,
    arpTimer,
    arpInput,
    NULL,
    NULL,
    NULL
};

