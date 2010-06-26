/*****************************************************************************
 * $Id: so_conn.c,v 1.5 2001/04/02 14:31:40 rf Exp $
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
 * ngSoConnect()
 *----------------------------------------------------------------------------
 * 04/11/98 - Regis Feneon
 * 12/01/99 -
 *  added support for unconfigured interfaces (if_addr=0)
 * 19/10/99 -
 *  convert ephemeral port number to network representation in so_lport
 * 09/02/2000 -
 *  added support for multicasts
 * 28/11/2000 -
 *  added test for duplicate socket pairs
 * 18/02/2001 -
 *  added NG_SO_DONTROUTE option test
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>


/*****************************************************************************
 * ngSoConnect()
 *****************************************************************************
 * connect a socket to a remote address
 * bind a local address and port if necessary
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  addr    address to connect to
 *  head    list of opened sockets
 *  eport   ephemeral port
 * Return value: 0 if ok or error code
 */



int ngSoConnect( NGsock *so, NGsockaddr *addr, NGsock *head, NGushort *eport)
{
    NGifnet *netp = NULL;
    NGuint gateway, faddr;

    /* we need a destination address and port */
    if( (addr->sin_port == 0) || (addr->sin_addr == NG_INADDR_ANY))
    {
          return( NG_EADDRNOTAVAIL);
    }

    /* at least one interface */
    if( ngIfList == NULL) {
        return( NG_EADDRNOTAVAIL);
    }

    /* test for broadcast or multicast destination */
    if( (addr->sin_addr == NG_INADDR_BROADCAST) ||
        NG_IN_MULTICAST( addr->sin_addr)) {
        /* ougoing interface selected ? */
        if( so->so_mcast_ifnetp)
          netp = so->so_mcast_ifnetp;
        else /* take first mcast/bcast interface */
          netp = ngIpRouteLookup( addr->sin_addr, &gateway,
                     so->so_options & NG_SO_DONTROUTE);
        /* no mcast interface available ? */
        if( netp == NULL) {
            return( NG_EADDRNOTAVAIL);
            }
        if( addr->sin_addr == NG_INADDR_BROADCAST) {
            if( (netp->if_flags & NG_IFF_BROADCAST) == 0)
             {
                return( NG_EADDRNOTAVAIL);
             }
            /* replace by interface broadcast address if possible */
            if( netp->if_addr)
                faddr = netp->if_broadaddr;
            else
                faddr = NG_INADDR_BROADCAST;
        }
        else {
            if( (netp->if_flags & NG_IFF_MULTICAST) == 0)
            {
              return( NG_EADDRNOTAVAIL);
             }
            faddr = addr->sin_addr;
        }
    }
    else {
        /* unicast destination address */
        faddr = addr->sin_addr;
        if( so->so_laddr == NG_INADDR_ANY) {
            /* find a route for dst address */
            netp = ngIpRouteLookup( faddr, &gateway,
                      so->so_options & NG_SO_DONTROUTE);
            if( netp == NULL) { 
                return( NG_EADDRNOTAVAIL);}
        }
    }

    if( so->so_lport) {
      NGsock *tso;
      /* test if socketpair does not exist */
      /*lint -e613 (Warning -- Possible use of null pointer )*/
      tso = ngSoLookup( head,
        (so->so_laddr == NG_INADDR_ANY) ? netp->if_addr : so->so_laddr,
        so->so_lport, faddr, addr->sin_port, 0);
      /*lint +e613 (Warning -- Possible use of null pointer )*/
      if( tso != NULL) {
               return( NG_EADDRINUSE);
        }
    }

    so->so_faddr = faddr;
    so->so_fport = addr->sin_port;

    /* set local address and port if not specified */
    if( so->so_laddr == NG_INADDR_ANY) {
        /* set interface address as source */
        /*lint -e613 (Warning -- Possible use of null pointer )*/
        so->so_laddr = netp->if_addr;
        /*lint +e613 (Warning -- Possible use of null pointer )*/
    }
    if( so->so_lport == 0) {
        /* choose an ephemeral port */
        ngSoFindEphemeral( so, head, eport);
    }

    return( NG_EOK);
}

