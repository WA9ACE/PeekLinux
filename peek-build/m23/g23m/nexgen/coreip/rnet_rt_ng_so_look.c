/*****************************************************************************
 * $Id: so_look.c,v 1.3 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Socket Level Internal Functions
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
 * ngSoLookup()
 *----------------------------------------------------------------------------
 * 04/11/98 - Regis Feneon
 * 12/12/98 -
 *  Changes in so parameter, now the last socket in the list points to NULL
 * 28/11/2000 -
 *  added wildok parameter
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSoLookup()
 *****************************************************************************
 * Find the socket corresponding to a pair of addresses
 *****************************************************************************
 * Parameters:
 *  head    circular list of opened sockets
 *  laddr   local ip address
 *  lport   local port
 *  faddr   foreign ip address
 *  fport   foreign port
 *  wildok  accept wildcard matchs
 * Return value: socket descriptor or NULL if no socket found
 */

NGsock *ngSoLookup( NGsock *head, NGuint laddr, NGushort lport,
                    NGuint faddr, NGuint fport, int wildok)
{
    NGsock *so, *match;
    int matchwild, wild;

    match = NULL;
    matchwild = 3;

    so = (NGsock *) head->so_node.next;
    for( ;so != head; so = (NGsock *) so->so_node.next) {

        /* local ports must match */
        if( so->so_lport != lport) continue;

        wild = 0;

        if( so->so_laddr != NG_INADDR_ANY) {
            if( laddr == NG_INADDR_ANY) wild++;
            else if( so->so_laddr != laddr) continue;
        }
        else if( laddr != NG_INADDR_ANY) wild++;

        if( so->so_faddr != NG_INADDR_ANY) {
            if( faddr == NG_INADDR_ANY) wild++;
            else if( (so->so_faddr != faddr) || (so->so_fport != fport)) continue;
        }
        else if( faddr != NG_INADDR_ANY) wild++;

        if( wild && !wildok) continue; /* wildcard not allowed */

        if( wild < matchwild) {
            match = so;
            matchwild = wild;
            if( matchwild == 0) break; /* exact match */
        }
    }
    return( match);
}

