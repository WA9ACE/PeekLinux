/*****************************************************************************
 * $Id: so_bind.c,v 1.4 2001/04/02 14:31:40 rf Exp $
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
 * ngSoBind()
 * ngSoFindEphemeral()
 *----------------------------------------------------------------------------
 * 04/11/98 - Regis Feneon
 * 12/01/99 -
 *  added support for unconfigured interfaces (if_addr=0)
 * 19/10/99 -
 *  convert ephemeral port number to network representation in so_lport
 * 08/06/2000 -
 *  changed ngSoFindEphemeral to return (void)
 * 28/11/2000 -
 *  special case for multicast address
 *  now call ngIpRouteLookup() to find bcast/mcast outgoing interface
 *  added test of socket options REUSEADDR and REUSEPORT
 *  and check if local port is used
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSoBind()
 *****************************************************************************
 * bind a local address and port to a socket
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  addr    address to bind
 *  head    list of opened sockets
 *  eport   ephemeral port
 * Return value: 0 if ok or error code
 */

int ngSoBind( NGsock *so, NGsockaddr *addr, NGsock *head, NGushort *eport)
{
    int reuse;
    NGifnet *netp;

    /* is socket already bound ? */
    if( (so->so_lport != 0) || (so->so_laddr != NG_INADDR_ANY))
      return( NG_EINVAL);

    if( addr) {
      reuse = so->so_options & NG_SO_REUSEPORT;
      /* test if addr is valid */
      if( NG_IN_MULTICAST( addr->sin_addr)) {
        reuse = so->so_options & (NG_SO_REUSEADDR|NG_SO_REUSEPORT);
      }
      else if( addr->sin_addr != NG_INADDR_ANY) {
        /* the address must be one of the local interface */
        netp = ngIfList;
        while( netp != NULL) {
          if( netp->if_addr == addr->sin_addr) {
            break;
          }
          netp = netp->if_next;
        }
        if( netp == NULL) return( NG_EADDRNOTAVAIL);
      }
      /* test if port is not used */
      if( addr->sin_port && !reuse) {
        NGsock *tso;
        tso = ngSoLookup( head, addr->sin_addr, addr->sin_port,
          NG_INADDR_ANY, 0,
          (so->so_options & (NG_SO_REUSEADDR|NG_SO_REUSEPORT)) == 0);
        if( tso != NULL) return( NG_EADDRINUSE);
      }
      so->so_laddr = addr->sin_addr;
      so->so_lport = addr->sin_port;
    }
    /* if no port specified choose an ephemeral port */
    if( so->so_lport == 0) {
      ngSoFindEphemeral( so, head, eport);
    }
    return( NG_EOK);
}

/*****************************************************************************
 * ngSoFindEphemeral()
 *****************************************************************************
 * find a free ephemeral port for the socket
 *****************************************************************************
 * Parameters:
 *  so      socket descriptor
 *  head    list of opened sockets
 *  eport   ephemeral port
 * Return value: none
 */

void ngSoFindEphemeral( NGsock *so, NGsock *head, NGushort *eport)
{
  NGsock *cso;

  while( 1) {
    so->so_lport = ngHTONS( *eport); /* 19/10/99 */
    (*eport)++;
    if( *eport >= IPPORT_USERRESERVED) *eport = IPPORT_RESERVED;
    /* look if local port is not already used */
    cso = (NGsock *) (head->so_node.next);
    while( cso != head) {
      if( (cso != so) && (cso->so_lport == so->so_lport))
        break; /* port already used, increment port number */
      /* next socket */
      cso = (NGsock *) (cso->so_node.next);
    }
    if( cso == head) break; /* no same socket pair found */
  }
}

