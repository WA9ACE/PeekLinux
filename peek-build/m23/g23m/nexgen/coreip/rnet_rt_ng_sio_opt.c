/*****************************************************************************
 * $Id: sio_opt.c,v 1.9 2002/04/23 10:20:04 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Asynchronous Sockets
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
 * ngSAIOOption()
 *----------------------------------------------------------------------------
 * 14/02/2001 - Regis Feneon
 *  original code from ngSoOption()
 * 18/02/2001 -
 *  added NG_IP_RECVDSTADDR option
 * 19/04/2001 -
 *  IP/HDRINCL option was missing
 * 09/04/2002 -
 *  NG_SO_NREAD: returns length of next datagram instead of so_rcv_cc
 *               for atomic protocols (udp/rawip)
 *****************************************************************************/

#include <ngip.h>
#include <ngsockio.h>

/*****************************************************************************
 * ngSAIOOption()
 *****************************************************************************
 * Set/Get a socket option.
 * socket level and ip level are processed in the routine, the other levels
 * options are passed to the protocol user option routine.
 *****************************************************************************
 * Parameters:
 *  so      pointer to socket control block
 *  level   NG_IOCTL_SOCKET, NG_IOCTL_IP
 *  option  option name | NG_SO_SET or NG_SO_GET
 *  optval  value
 *  optlen  size of optval
 * Return value: NG_EOK or negative error code
 */

int ngSAIOOption( NGsock *so, int level, int option, void *optval,
                  int *optlen)
{
  u_long timeo = 0;
  NGifnet *netp;
#ifdef NG_IGMP_SUPPORTED
  int i, iempty;
  NGinmulti *inmp;
#endif

  if( level == (int) NG_IOCTL_SOCKET) {
    /*
     * Set Socket Level Option
     */
    switch( option) {
    /* set options and buffer sizes */
    case NG_SO_SET|NG_SO_DEBUG:
    case NG_SO_SET|NG_SO_REUSEADDR:
    case NG_SO_SET|NG_SO_REUSEPORT:
    case NG_SO_SET|NG_SO_KEEPALIVE:
    case NG_SO_SET|NG_SO_DONTROUTE:
    case NG_SO_SET|NG_SO_BROADCAST:
    case NG_SO_SET|NG_SO_OOBINLINE:
    case NG_SO_SET|NG_SO_NBIO:
    case NG_SO_SET|NG_SO_SNDBUF:
    case NG_SO_SET|NG_SO_RCVBUF:
    case NG_SO_SET|NG_SO_SNDLOWAT:
    case NG_SO_SET|NG_SO_RCVLOWAT:
      if( *optlen != sizeof( int)) return( NG_EINVAL);
      switch( option) {
      case NG_SO_SET|NG_SO_NBIO:
          if( *((int *) optval)) so->so_state |= NG_SS_NBIO;
          else so->so_state &= ~NG_SS_NBIO;
          break;
      case NG_SO_SET|NG_SO_SNDBUF:
          so->so_snd_hiwat = *((int *) optval);
          break;
      case NG_SO_SET|NG_SO_RCVBUF:
          so->so_rcv_hiwat = *((int *) optval);
          break;
      case NG_SO_SET|NG_SO_SNDLOWAT:
          so->so_snd_lowat = *((int *) optval);
          break;
      case NG_SO_SET|NG_SO_RCVLOWAT:
          so->so_rcv_lowat = *((int *) optval);
          break;
      default: /* other flags (so_options field) */
          option &= ~NG_SO_SET;
          if( *((int *) optval)) so->so_options |= option;
          else so->so_options &= ~((unsigned)option);
          break;
      }
      break;
    /* linger time */
    case NG_SO_SET|NG_SO_LINGER:
      if( *optlen != sizeof( NGlinger)) return( NG_EINVAL);
      so->so_linger = ((NGlinger *) optval)->l_linger;
      if( ((NGlinger *) optval)->l_onoff)
          so->so_options |= NG_SO_LINGER;
      else
          so->so_options &= ~NG_SO_LINGER;
      break;
    /* timeouts */
    case NG_SO_SET|NG_SO_SNDTIMEO:
    case NG_SO_SET|NG_SO_RCVTIMEO:
      if( *optlen != sizeof( NGtimeval)) return( NG_EINVAL);
      /* transform timeout to clock ticks */
      /* in milliseconds */
      timeo = ((u_long) ((NGtimeval *) optval)->tv_sec)*1000UL
              + ((u_long) ((NGtimeval *) optval)->tv_usec)/1000UL;
      /* in system clock ticks */
      timeo = timeo*ngOSClockGetFreq()/1000UL;
      switch( option) {
      case NG_SO_SET|NG_SO_SNDTIMEO:
          so->so_snd_timeo = timeo;
          break;
      case NG_SO_SET|NG_SO_RCVTIMEO:
          so->so_rcv_timeo = timeo;
          break;
      }
      break;
    /*
     * Get Socket Level Option
     */
    /* options and buffer sizes */
    case NG_SO_GET|NG_SO_DEBUG:
    case NG_SO_GET|NG_SO_REUSEADDR:
    case NG_SO_GET|NG_SO_REUSEPORT:
    case NG_SO_GET|NG_SO_KEEPALIVE:
    case NG_SO_GET|NG_SO_DONTROUTE:
    case NG_SO_GET|NG_SO_BROADCAST:
    case NG_SO_GET|NG_SO_OOBINLINE:
    case NG_SO_GET|NG_SO_NBIO:
    case NG_SO_GET|NG_SO_ATMARK:
    case NG_SO_GET|NG_SO_SNDBUF:
    case NG_SO_GET|NG_SO_RCVBUF:
    case NG_SO_GET|NG_SO_SNDLOWAT:
    case NG_SO_GET|NG_SO_RCVLOWAT:
    case NG_SO_GET|NG_SO_NREAD:
    case NG_SO_GET|NG_SO_ERROR:
    case NG_SO_GET|NG_SO_TYPE:
      if( (unsigned)(*optlen) < sizeof( int)) return( NG_EINVAL);
      *optlen = sizeof( int);
      switch( option) {
      case NG_SO_GET|NG_SO_NBIO:
          *((int *) optval) = so->so_state & NG_SS_NBIO;
          break;
      case NG_SO_GET|NG_SO_ATMARK:
          *((int *) optval) = so->so_state & NG_SS_RCVATMARK;
          break;
      case NG_SO_GET|NG_SO_SNDBUF:
          *((int *) optval) = so->so_snd_hiwat;
          break;
      case NG_SO_GET|NG_SO_RCVBUF:
          *((int *) optval) = so->so_rcv_hiwat;
          break;
      case NG_SO_GET|NG_SO_SNDLOWAT:
          *((int *) optval) = so->so_snd_lowat;
          break;
      case NG_SO_GET|NG_SO_RCVLOWAT:
          *((int *) optval) = so->so_rcv_lowat;
          break;
      case NG_SO_GET|NG_SO_NREAD:
          if( so->so_proto->pr_flags & NG_PR_ATOMIC) {
            NGbuf *bufp;
            int nread;
            nread = 0;
            /* returns length of next buffer to be read */
            NG_QUEUE_PEEK( &so->so_rcv_q, bufp);
            while( bufp != NULL) {
              nread += bufp->buf_datalen;
              bufp = bufp->buf_next;
            }
            *((int *) optval) = nread;
          }
          else {
            /* returns total nb of bytes in receive buffer */
            *((int *) optval) = so->so_rcv_cc;
          }
          break;
      case NG_SO_GET|NG_SO_ERROR:
          *((int *) optval) = so->so_error;
          so->so_error = 0;
          break;
      case NG_SO_GET|NG_SO_TYPE:
          *((int *) optval) = so->so_proto->pr_type;
          break;
      default: /* other flags (so_options field) */
          *((int *) optval) = so->so_options & option;
          break;
      }
      break;
    /* linger time */
    case NG_SO_GET|NG_SO_LINGER:
      if( (unsigned)(*optlen) < sizeof( NGlinger)) return( NG_EINVAL);
      *optlen = sizeof( NGlinger);
      ((NGlinger *) optval)->l_onoff = so->so_options & NG_SO_LINGER;
      ((NGlinger *) optval)->l_linger = so->so_linger;
      break;
    /* timeouts */
    case NG_SO_GET|NG_SO_SNDTIMEO:
    case NG_SO_GET|NG_SO_RCVTIMEO:
      if( (unsigned)(*optlen) < sizeof( NGtimeval)) return( NG_EINVAL);
      *optlen = sizeof( NGtimeval);
      switch( option) {
      case NG_SO_GET|NG_SO_SNDTIMEO:
          timeo = so->so_snd_timeo;
          break;
      case NG_SO_GET|NG_SO_RCVTIMEO:
          timeo = so->so_rcv_timeo;
          break;
      }
      /* transform timeout in clock ticks to second/microsec */
      /* timeo in milliseconds */
      timeo = timeo*1000UL/ngOSClockGetFreq();
      ((NGtimeval *) optval)->tv_sec = timeo/1000UL;
      ((NGtimeval *) optval)->tv_usec = 1000UL*(timeo
                       - ((NGtimeval *) optval)->tv_sec*1000UL);
      break;
    /* socket addresses */
    case NG_SO_GET|NG_SO_SOCKNAME:
    case NG_SO_GET|NG_SO_PEERNAME:
      if( (unsigned)(*optlen) < sizeof( NGsockaddr)) return( NG_EINVAL);
      *optlen = sizeof( NGsockaddr);
      ((NGsockaddr *) optval)->sin_len = sizeof( NGsockaddr);
      ((NGsockaddr *) optval)->sin_family = NG_AF_INET;
      if( option == (NG_SO_GET|NG_SO_SOCKNAME)) {
          ((NGsockaddr *) optval)->sin_addr = so->so_laddr;
          ((NGsockaddr *) optval)->sin_port = so->so_lport;
      }
      else {
          ((NGsockaddr *) optval)->sin_addr = so->so_faddr;
          ((NGsockaddr *) optval)->sin_port = so->so_fport;
      }
      break;
    default:
        return( NG_ENOPROTOOPT);
    }
  }
  else if( level == NG_IOCTL_IP) {
    /*
     * IP level options
     */
    switch( option) {
    case NG_SO_SET|NG_IP_TOS:
    case NG_SO_SET|NG_IP_TTL:
    case NG_SO_SET|NG_IP_MULTICAST_TTL:
    case NG_SO_SET|NG_IP_MULTICAST_LOOP:
    case NG_SO_SET|NG_IP_DONTFRAG:
    case NG_SO_SET|NG_IP_RECVDSTADDR:
    case NG_SO_SET|NG_IP_HDRINCL:
      if( *optlen != sizeof( int)) return( NG_EINVAL);
      switch( option) {
      case NG_SO_SET|NG_IP_TOS:
        so->so_tos = *((int *) optval);
        break;
      case NG_SO_SET|NG_IP_TTL:
        so->so_ttl = *((int *) optval);
        break;
      case NG_SO_SET|NG_IP_MULTICAST_TTL:
        so->so_mcast_ttl = *((int *) optval);
        break;
      case NG_SO_SET|NG_IP_MULTICAST_LOOP:
        if( *((int *) optval))
          so->so_options |= NG_SO_MCASTLOOP;
        else
          so->so_options &= ~NG_SO_MCASTLOOP;
        break;
      case NG_SO_SET|NG_IP_DONTFRAG:
        if( *((int *) optval))
          so->so_options |= NG_SO_DONTFRAG;
        else
          so->so_options &= ~NG_SO_DONTFRAG;
        break;
      case NG_SO_SET|NG_IP_RECVDSTADDR:
        if( *((int *) optval))
          so->so_options |= NG_SO_RECVDSTADDR;
        else
          so->so_options &= ~NG_SO_RECVDSTADDR;
        break;
      case NG_SO_SET|NG_IP_HDRINCL:
        if( *((int *) optval))
          so->so_options |= NG_SO_HDRINCL;
        else
          so->so_options &= ~NG_SO_HDRINCL;
        break;
      }
      break;
    case NG_SO_SET|NG_IP_MULTICAST_IF:
      if( (unsigned)(*optlen) < sizeof( NGsockaddr)) return( NG_EINVAL);
      if( ((NGsockaddr *) optval)->sin_addr == NG_INADDR_ANY) {
        so->so_mcast_ifnetp = NULL;
      }
      else {
        /* try to find the interface with the selected address */
        netp = ngIfList;
        while( netp != NULL) {
          if( netp->if_addr == ((NGsockaddr *) optval)->sin_addr)
            break;
          netp = netp->if_next;
        }
        if( (netp == NULL) ||
            ((netp->if_flags & (NG_IFF_MULTICAST|NG_IFF_BROADCAST)) == 0))
          return( NG_EADDRNOTAVAIL);
        so->so_mcast_ifnetp = netp;
      }
      break;
    case NG_SO_SET|NG_IP_MULTICAST_IFP:
      if( *optlen != sizeof( NGifnet *)) return( NG_EINVAL);
      netp = *((NGifnet **) optval);
      if( (netp != NULL) &&
          ((netp->if_flags & (NG_IFF_MULTICAST|NG_IFF_BROADCAST)) == 0) )
        return( NG_EADDRNOTAVAIL);
      so->so_mcast_ifnetp = netp;
      break;

#ifdef NG_IGMP_SUPPORTED
    case NG_SO_SET|NG_IP_ADD_MEMBERSHIP:
    case NG_SO_SET|NG_IP_ADD_MEMBERSHIP_IFP:
    case NG_SO_SET|NG_IP_DROP_MEMBERSHIP:
    case NG_SO_SET|NG_IP_DROP_MEMBERSHIP_IFP:
      if( *optlen != sizeof( NGip_mreq)) return( NG_EINVAL);
      /* the group must be a multicast address */
      if( !NG_IN_MULTICAST( ((NGip_mreq *) optval)->imr_multiaddr))
        return( NG_EINVAL);
      /* find the interface */
      if( (option == (int) (NG_SO_SET|NG_IP_ADD_MEMBERSHIP)) ||
          (option == (int) (NG_SO_SET|NG_IP_DROP_MEMBERSHIP)) ) {
        netp = ngIfList;
        while( netp) {
          if( netp->if_addr == ((NGip_mreq *) optval)->imr_interface)
            break;
          netp = netp->if_next;
        }
      }
      else {
        netp = ((NGip_mreq *) optval)->imr_ifp;
      }
      /* is the interface valid ? */
      if( (netp == NULL) || ((netp->if_flags & NG_IFF_MULTICAST) == 0))
        return( NG_EADDRNOTAVAIL);

      if( (option == (int) (NG_SO_SET|NG_IP_ADD_MEMBERSHIP)) ||
          (option == (int) (NG_SO_SET|NG_IP_ADD_MEMBERSHIP_IFP)) ) {
        /* add a new membership... */
        /* all-hosts group is always joined */
        if( ((NGip_mreq *) optval)->imr_multiaddr == NG_INADDR_ALLHOSTS_GROUP)
          return( NG_EADDRINUSE);
        /* see if the membership already exist */
        /* else find an empty slot */
        iempty = -1;
        for( i=0; i<NG_SOCK_MAX_MEMBERSHIPS; i++) {
          if( so->so_mcast_memberships[i] == NULL) {
            if( iempty < 0) iempty = i;
          }
          else {
            if( (so->so_mcast_memberships[i]->inm_ifnetp == netp) &&
                (so->so_mcast_memberships[i]->inm_addr ==
                  ((NGip_mreq *) optval)->imr_multiaddr) )
              return( NG_EADDRINUSE);
          }
        }
        if( iempty < 0)
          return( NG_ETOOMANYREFS); /* no empty slot... */
        /* look if the group is already on the interface */
        inmp = netp->if_multiaddrs;
        while( inmp) {
          if( inmp->inm_addr == ((NGip_mreq *) optval)->imr_multiaddr)
            break;
          inmp = inmp->inm_next;
        }
        if( inmp == NULL) {
          /* join a new igmp group */
          inmp = ngIgmpJoinGroup( netp, ((NGip_mreq *) optval)->imr_multiaddr);
          if( inmp == NULL)
            return( NG_ENOBUFS); /* no free group entry */
        }
        /* attach the group to the socket */
        so->so_mcast_memberships[iempty] = inmp;
        inmp->inm_refcount++;
      }
      else {
        /* drop a membership... */
        /* find the entry */
        for( i=0; i<NG_SOCK_MAX_MEMBERSHIPS; i++) {
          inmp = so->so_mcast_memberships[i];
          if( (inmp != NULL) &&
              (inmp->inm_ifnetp == netp) &&
              (inmp->inm_addr == ((NGip_mreq *) optval)->imr_multiaddr) ) {
            /* detach the group */
            so->so_mcast_memberships[i] = NULL;
            /* if no more reference, leave the group */
            if( --inmp->inm_refcount == 0) ngIgmpLeaveGroup( inmp);
            break;
          }
        }
        /* no entry found */
        if( i == NG_SOCK_MAX_MEMBERSHIPS)
          return( NG_EADDRNOTAVAIL);
      }
      break;
#endif

    case NG_SO_GET|NG_IP_TOS:
    case NG_SO_GET|NG_IP_TTL:
    case NG_SO_GET|NG_IP_MULTICAST_TTL:
    case NG_SO_GET|NG_IP_MULTICAST_LOOP:
    case NG_SO_GET|NG_IP_DONTFRAG:
    case NG_SO_GET|NG_IP_RECVDSTADDR:
    case NG_SO_GET|NG_IP_HDRINCL:
      if( (unsigned)(*optlen) < sizeof( int)) return( NG_EINVAL);
      *optlen = sizeof( int);
      switch( option) {
      case NG_SO_GET|NG_IP_TOS:
        *((int *) optval) = so->so_tos;
        break;
      case NG_SO_GET|NG_IP_TTL:
        *((int *) optval) = so->so_ttl;
        break;
      case NG_SO_GET|NG_IP_MULTICAST_TTL:
        *((int *) optval) = so->so_mcast_ttl;
        break;
      case NG_SO_GET|NG_IP_MULTICAST_LOOP:
        *((int *) optval) = so->so_options & NG_SO_MCASTLOOP;
        break;
      case NG_SO_GET|NG_IP_DONTFRAG:
        *((int *) optval) = so->so_options & NG_SO_DONTFRAG;
        break;
      case NG_SO_GET|NG_IP_RECVDSTADDR:
        *((int *) optval) = so->so_options & NG_SO_RECVDSTADDR;
        break;
      case NG_SO_GET|NG_IP_HDRINCL:
        *((int *) optval) = so->so_options & NG_SO_HDRINCL;
        break;
      }
      break;
    case NG_SO_GET|NG_IP_MULTICAST_IF:
        if( (unsigned)(*optlen) < sizeof( NGsockaddr)) return( NG_EINVAL);
        *optlen = sizeof( NGsockaddr);
        ((NGsockaddr *) optval)->sin_len = sizeof( NGsockaddr);
        ((NGsockaddr *) optval)->sin_family = NG_AF_INET;
        ((NGsockaddr *) optval)->sin_port = 0;
        if( so->so_mcast_ifnetp) {
          ((NGsockaddr *) optval)->sin_addr =
              ((NGifnet *) so->so_mcast_ifnetp)->if_addr;
        }
        else {
          ((NGsockaddr *) optval)->sin_addr = NG_INADDR_ANY;
        }
        break;
    case NG_SO_GET|NG_IP_MULTICAST_IFP:
        if( (unsigned)(*optlen) < sizeof( NGifnet *)) return( NG_EINVAL);
        *optlen = sizeof( NGifnet *);
        *((NGifnet **) optval) = so->so_mcast_ifnetp;
        break;
    default:
        return( NG_ENOPROTOOPT);
    }
  }
  else if( ((NGproto *) so->so_proto)->pr_useropt_f) {
    /* call transport protocol option routine for other levels */
    return( ( (int (*)( NGsock *, u_int, u_int, void *, int *))
     (((NGproto *) so->so_proto)->pr_useropt_f))( so, level, option,
                                                    optval, optlen));
  }
  else return( NG_ENOPROTOOPT);
  return( NG_EOK);
}

