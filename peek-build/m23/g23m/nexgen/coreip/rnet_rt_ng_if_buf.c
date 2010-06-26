/*****************************************************************************
 * $Id: if_buf.c,v 1.4 2002/04/23 10:39:34 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Generic Interface Functions
 * Internal buffer management
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2002 NexGen Software.
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
 * ngIfBufAttach()
 * ngIfBufDetach()
 *----------------------------------------------------------------------------
 * 15/02/2000 - Regis Feneon
 * 19/07/2000 -
 *  ethernet routines moved to generic interface level
 * 23/04/2002 -
 *  ngIfBufDetach(): added (NGubyte *) cast
 *****************************************************************************/

#include <ngip.h>
#include <ngnet.h>

/*****************************************************************************
 * ngIfBufAttach()
 *****************************************************************************
 * Keep a reference to a message buffer
 *****************************************************************************
 * Parameters:
 *  netp    network interface
 *  bufp    message buffer
 * Return value: none
 */

void ngIfBufAttach( NGifnet *netp, NGbuf *bufp)
{
    int ictl;

    ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE);
    /* attach buffer into list */
    NG_NODE_IN( &netp->if_buflist, bufp);
    ngOSIntrCtl( ictl);
}

/*****************************************************************************
 * ngIfBufDetach()
 *****************************************************************************
 * Find a message buffer referenced by its data pointer
 *****************************************************************************
 * Parameters:
 *  netp    network interface
 *  datap   data pointer
 * Return value: pointer to message buffer or NULL if no buffer found
 */

NGbuf *ngIfBufDetach( NGifnet *netp, void *datap)
{
    NGbuf *bufp;
    int ictl;

    ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE);
    bufp = (NGbuf *) netp->if_buflist.next;
    while( 1) {
        if( bufp == (NGbuf *) &netp->if_buflist) {
            ngOSIntrCtl( ictl);
            return( NULL); /* buffer not found... */
        }
        if( bufp->buf_datap == (NGubyte *) datap) {
            /* detach buffer */
            NG_NODE_DETACH( bufp);
            ngOSIntrCtl( ictl);
            return( bufp); /* buffer found */
        }
        bufp = (NGbuf *) bufp->buf_node.next;
    }
}

