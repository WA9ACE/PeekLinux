/*****************************************************************************
 * $Id: eth_inp.c,v 1.3 2001/11/20 14:36:07 rf Exp $
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
 * ngEtherInput()
 *----------------------------------------------------------------------------
 * 08/02/2000 - Regis Feneon
 * 19/07/2000 -
 *  added fromisr parameter
 * 25/07/2000 -
 *  added PPPoE handling
 *  corrected multicast address test
 * 06/11/2000 -
 *  corrected multicast/broadcast address test
 * 20/11/2001 -
 *  sizeof(NGetherhdr) replaced with ETHER_HDR_LEN
 *****************************************************************************/

#include <ngip.h>
#include <ngeth.h>
#include <ngip/ethernet.h>

/*****************************************************************************
 * ngEtherInput()
 *****************************************************************************
 * Enqueue ethernet input buffer
 *  test header protocol
 *  test for broadcast or multicast destination
 *****************************************************************************
 * Parameters:
 *  netp        network interface
 *  bufp        received message buffer
 *  fromisr     non-zero if called from a isr
 * Return value: 0 or error code
 */

void ngEtherInput( NGifnet *netp, NGbuf *bufp, int fromisr)
{
    NGubyte *p;
    unsigned int etype;

    bufp->buf_flags &= ~(NG_BUFF_MCAST|NG_BUFF_BCAST|NG_BUFF_PROTO);

    /* protocol */
    etype = ((NGetherhdr *) bufp->buf_datap)->ether_type;

    switch( etype) {
    case ngConstHTONS( ETHERTYPE_IP):
        bufp->buf_flags |= NG_PROTO_IP;
        break;
    case ngConstHTONS( ETHERTYPE_ARP):
        bufp->buf_flags |= NG_PROTO_ARP;
        break;
    case ngConstHTONS( ETHERTYPE_REVARP):
        bufp->buf_flags |= NG_PROTO_RARP;
        break;
    case ngConstHTONS( ETHERTYPE_PPPOEDISC):
    case ngConstHTONS( ETHERTYPE_PPPOE):
        bufp->buf_flags |= NG_PROTO_PPPOE;
        break;
    default:
        /* unknown protocol */
        bufp->buf_flags |= NG_PROTO_UNKNOWN;
    }

    /* is the message multicast/broadcast ? */
    p = bufp->buf_datap;
    /* test Individual/Group bit */
    if( *p & 0x01) {
      if( (*p++ == 0xff) && (*p++ == 0xff) && (*p++ == 0xff) &&
          (*p++ == 0xff) && (*p++ == 0xff) && (*p++ == 0xff))
        bufp->buf_flags |= NG_BUFF_BCAST;
      else
        bufp->buf_flags |= NG_BUFF_MCAST;
    }

    /* adjust pointers to data only */
    bufp->buf_datap += ETHER_HDR_LEN;
    bufp->buf_datalen -= ETHER_HDR_LEN;

    /* enqueue buffer */
    ngIfGenInput( netp, bufp, fromisr);
}

