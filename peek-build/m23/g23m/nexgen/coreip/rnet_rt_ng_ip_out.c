/*****************************************************************************
 * $Id: ip_out.c,v 1.8.2.1 2002/10/28 18:49:49 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet Protocol Functions
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
 * ngIpOutput()
 *----------------------------------------------------------------------------
 * 22/09/98 - Regis Feneon
 * 13/01/99 -
 *  added support for unconfigured interfaces (if_addr==0)
 * 04/02/99 -
 *  full fragmentation support added
 * 29/05/99 -
 *  iov_copy moved to iovcpy.c (renamed ngIovCpy())
 * 09/02/2000 -
 *  added support for multicasts
 * 18/02/2000 - Adrien Felon
 *  including new pieces of code in ngIpOutput() for IP forwarding support
 * 24/02/2000 - Regis Feneon
 *  set gateway value when multicasting
 * 07/07/2000 -
 *  dont test broadcast for point-to-point interface
 * 12/09/2000 -
 *  select first broadcast or multicast capable interface if destination
 *   is limited broadcast or multicast
 *   (not necessary the first interface in the list)
 * 16/11/2000 -
 *  added IP statistics
 * 27/11/2000 -
 *  IpId moved to ip.c
 * 16/01/2001 -
 *  loopback of multicast datagrams
 * 23/01/2001 -
 *  corrections in NGiphdr structure
 * 18/02/2001 -
 *  added DONTROUTE flag
 *  support for tcp output fragmentation (buffers with dontfree flags)
 * 04/12/2001 -
 *  evaluation version - limit output TTL to NG_IP_EVAL_MAX_TTL
 * 28/10/2002 -
 *  removed some warnings with msvc
 *****************************************************************************/

#include <ngip.h>
#include <ngip/ip.h>
#include <ngip/icmp.h>
#include <ngip/udp.h>

#ifdef NG_NATFWD_SUPPORTED
#include <ngnat.h>
#endif

#ifdef NG_DEBUG
extern const char *icmp_type_str[];
#endif

#ifdef NG_EVAL_VERSION
#define NG_IP_EVAL_TTL_MAX 2
#endif

/*****************************************************************************
 * ngIpOutput()
 *****************************************************************************
 * Send a IP message to the link layer
 *****************************************************************************
 * Parameters:
 *  bufp        message to send
 *  flags       flags (NG_IPOUTPUT_IPFWD or NG_IPOUTPUT_NATFWD)
 * Return value: 0 if ok or error code
 */

int ngIpOutput( NGbuf *bufp, int flags)
{

#if !defined(NG_IPFWD_SUPPORTED) && defined(NG_NATFWD_SUPPORTED)
#error "NAT supports require NG_IPFWD_SUPPORTED to be defined"
#endif

  NGiphdr *ip;
  int hlen, err;
  NGuint gateway;
  NGifnet *out_netp; /* outgoing interface */
#ifdef NG_IGMP_SUPPORTED
  int mloop = 0;
#endif

#ifdef NG_IPFWD_SUPPORTED
  /* We might send ICMP messages if IP forwarding is enable. We use
   * "icmp_type" var to know wether or not an ICMP msg needs to be
   * sent at the very end of this function. The ECHOREPLY msg will
   * NEVER be sent by this function so it is ok to use value
   * NG_ICMP_ECHOREPLY (0) as default value for "icmp_type".
   */
  NGicmp *icp;
  NGuint dest;
  int dlen, icmp_code, icmp_type;
  NGifnet *in_netp; /* shortcut for incoming interface */
  in_netp = (NGifnet*)bufp->buf_ifnetp;
  icmp_type = 0;
  icmp_code = 0;
#endif

  ip = (NGiphdr *) bufp->buf_datap;
  hlen = (ip->ip_hlv & NG_IPHDRLEN_MASK)<<2;

  gateway = 0;

  if( (ip->ip_dst == NG_INADDR_BROADCAST) || NG_IN_CLASSD( ip->ip_dst)) {
    if( ip->ip_dst == NG_INADDR_BROADCAST)
      /* limited broadcast */
      bufp->buf_flags |= NG_BUFF_BCAST;
    else
      /* multicast */
      bufp->buf_flags |= NG_BUFF_MCAST;
    /* take selected interface or default (first interface) */
    if( bufp->buf_flags & NG_BUFF_SETIF) {
      out_netp = bufp->buf_ifnetp;
    }
    else {
      out_netp = ngIfList; /* broadcast on the first interface only */
      while( out_netp != NULL) { /* 12/09/2000 find the first bcast interf. */
        if( ((bufp->buf_flags & NG_BUFF_BCAST) &&
             (out_netp->if_flags & NG_IFF_BROADCAST)) ||
            ((bufp->buf_flags & NG_BUFF_MCAST) &&
             (out_netp->if_flags & NG_IFF_MULTICAST)) ) break;
        out_netp = out_netp->if_next;
      }
      if( out_netp == NULL) {
        err = NG_ENETUNREACH;
#ifdef NG_IPSTATS_SUPPORTED
        ngIpStat.ips_noroute++;
#endif
        goto bad;
      }
    }
    gateway = ip->ip_dst;
  }
  else {
    bufp->buf_flags &= ~(NG_BUFF_BCAST|NG_BUFF_MCAST);
    if( ip->ip_dst == 0) {
      err = NG_EHOSTUNREACH;
#ifdef NG_IPSTATS_SUPPORTED
      ngIpStat.ips_noroute++;
#endif
      goto bad;
    }

    /* routing */
    out_netp = ngIpRouteLookup( ip->ip_dst, &gateway,
                                flags & NG_IPOUTPUT_DONTROUTE);
    if( out_netp == NULL) {
      err = NG_EHOSTUNREACH;
#ifdef NG_IPFWD_SUPPORTED
      /* send ICMP error UNREACH_HOST if packet is forwarded */
      if (flags & NG_IPOUTPUT_IPFWD) {
        icmp_type = ICMP_UNREACH;
        icmp_code = ICMP_UNREACH_HOST;
        goto send_icmp;
      }
#endif
#ifdef NG_IPSTATS_SUPPORTED
      ngIpStat.ips_noroute++;
#endif
      goto bad;
    }

    /* broadcast ? */
    if( !(out_netp->if_flags & NG_IFF_POINTOPOINT) &&
        ((gateway | out_netp->if_subnetmask) == NG_INADDR_BROADCAST)) {
      bufp->buf_flags |= NG_BUFF_BCAST;
    }
  }

  /* test if interface is up */
  if( !(out_netp->if_flags & NG_IFF_UP)) {
    err = NG_ENETDOWN;
#ifdef NG_IPFWD_SUPPORTED
    /* send ICMP error NETDOWN if packet is forwarded */
    if (flags & NG_IPOUTPUT_IPFWD) {
      icmp_type = ICMP_UNREACH;
      icmp_code = ICMP_UNREACH_NET;
      goto send_icmp;
    }
#endif
#ifdef NG_IPSTATS_SUPPORTED
    ngIpStat.ips_noroute++;
#endif
    goto bad;
  }

#ifdef NG_IPFWD_SUPPORTED

  if (!in_netp)
    flags = 0;
  else { /* should be replaced by "if (flags) {" */

    /* Send ICMP redirect if forwarding packet to incoming interface.
     * We are an invalid gateway, valid one is in "gateway" var.
     */
    if ( (flags & NG_IPOUTPUT_IPFWD) && (in_netp == out_netp) ) {
#ifdef NG_DEBUG
      ngDebug(NG_DBG_IP, NG_DBG_OUTPUT, 0,
              "Host redirection: because %s is both in and out interface\n",
              out_netp->if_name);
#endif
      icmp_type = ICMP_REDIRECT;
      icmp_code = ICMP_REDIRECT_HOST;
      goto send_icmp;
    }

#ifdef NG_NATFWD_SUPPORTED

    /*
     * Masquerading packet coming from private net going into pub net
     */
    if (
        !(NG_IFF_EXTNAT & in_netp->if_flags) &&
        (NG_IFF_EXTNAT & out_netp->if_flags)
    ) {
      if (NG_EOK != (err = ngNatOutput(bufp, out_netp, &flags))) {
#ifdef NG_IPSTATS_SUPPORTED
        ngIpStat.ips_cantforward++;
#endif
        goto bad;
      }
      flags |= NG_IPOUTPUT_NATFWD;
    }

    /*
     * Checking if we need to cross NAT border
     */
    if (
        (NG_IFF_EXTNAT&(in_netp->if_flags)) !=
        (NG_IFF_EXTNAT&(out_netp->if_flags))
    ) {
      if (flags & NG_IPOUTPUT_NATFWD) {
        /* Clear normal IP forwarding flag */
        flags &= ~NG_IPOUTPUT_IPFWD;
#ifdef NG_DEBUG
        ngDebug(NG_DBG_NAT, NG_DBG_INIT, 0, "NAT Forwarding %s -> %s (%s)",
                in_netp->if_name, out_netp->if_name,
                (NG_IFF_EXTNAT & out_netp->if_flags)?"priv->pub":"pub->priv");
#endif
      } else {
        /* Dangerous stuff is happening! */
        if (NG_IFF_EXTNAT&(in_netp->if_flags)) {
          err = NG_EACCES;
#ifdef NG_DEBUG
          ngDebug(NG_DBG_NAT, NG_DBG_INIT, 0,
                  "Discarding access to private network");
#endif
        } else {
          err = NG_EINVAL;
#ifdef NG_DEBUG
          ngDebug(NG_DBG_NAT, NG_DBG_INIT, 0, "Something wrong.....");
#endif
        }
#ifdef NG_IPSTATS_SUPPORTED
        ngIpStat.ips_cantforward++;
#endif
        goto bad;
      }
    }

#ifdef NG_DEBUG
    if ( (flags & NG_IPOUTPUT_IPFWD) && (out_netp != in_netp) ) {
      ngDebug(NG_DBG_IP, NG_DBG_OUTPUT, 0, "Normal IP forwarding %s -> %s",
              in_netp->if_name, out_netp->if_name);
    }
#endif

#endif /* NG_NATFWD_SUPPORTED */
  }
#endif /* NG_IPFWD_SUPPORTED */

  /* Now ok to set outgoing interface in buffer */
  bufp->buf_ifnetp = out_netp;

#ifdef NG_IGMP_SUPPORTED
  /* do we need to loopback the datagram ? */
  /* do not loopback forwarded datagrams */
  mloop = 0;
  if( (bufp->buf_flags & NG_BUFF_MCAST) &&
      ((flags & (NG_IPOUTPUT_IPFWD|NG_IPOUTPUT_NATFWD|NG_IPOUTPUT_MCASTLOOP))
       == NG_IPOUTPUT_MCASTLOOP) ) {
    /* all-hosts address ? */
    if( ip->ip_dst == NG_INADDR_ALLHOSTS_GROUP) {
      mloop = 1;
    }
    else {
      /* the interface must be member of the group */
      NGinmulti *inmp;
      inmp = out_netp->if_multiaddrs;
      while( inmp != NULL) {
        if( inmp->inm_addr == ip->ip_dst) {
          mloop = 1;
          break;
        }
        inmp = inmp->inm_next;
      }
    }
  }
#endif

  /* Filling in IP protocol fields in buffer if necessary */
  if(
     !(bufp->buf_flags & NG_BUFF_RAW)
#ifdef NG_IPFWD_SUPPORTED
     && !(flags & NG_IPOUTPUT_IPFWD)
#endif
  ) {

    /* select interface address if no source address */
    if( ip->ip_src == INADDR_ANY) ip->ip_src = out_netp->if_addr;

    /* initialize fields */
    ip->ip_off &= IPOFF_DF;
    ip->ip_hlv = (ip->ip_hlv & NG_IPHDRLEN_MASK) | NG_IPVER_IPV4;
    ip->ip_id = ngIpId++;
    ip->ip_id = ngHTONS( ip->ip_id);

    /* broadcast ? */
    if( bufp->buf_flags & NG_BUFF_BCAST) {
      /* is broadcast allowed on this interface ? */
      if( !(out_netp->if_flags & NG_IFF_BROADCAST)) {
        err = NG_EADDRNOTAVAIL;
#ifdef NG_IPSTATS_SUPPORTED
        ngIpStat.ips_noroute++;
#endif
        goto bad;
      }
    }
  }

#ifdef NG_EVAL_VERSION
  /* limit TTL */
  if( ip->ip_ttl > NG_IP_EVAL_TTL_MAX)
    ip->ip_ttl = NG_IP_EVAL_TTL_MAX;
#endif

  /* fragmentation needed ? */
  if( ip->ip_len > out_netp->if_mtu) {
#ifdef NG_IPFRAG_SUPPORTED
    int f_hlen, tlen;
    u_int f_len, f_firstlen, f_off;
    NGiphdr *f_ip;
    NGbuf *f_bufp, *tbufp;

    /* don't allow fragmentation on broadcast NG_BUFF_BCAST */
    /* IPOFF_DF flag must not be set */
    if( (bufp->buf_flags & NG_BUFF_BCAST) ||
        (ip->ip_off & IPOFF_DF)) {
      err = NG_EMSGSIZE;
#ifdef NG_IPSTATS_SUPPORTED
      ngIpStat.ips_cantfrag++;
#endif
      goto bad;
    }

    /* determine fragment data size (rounded to 8 byte boundary) */
    f_len = (out_netp->if_mtu - hlen) & ~7;
    f_firstlen = f_len;

    /* if first buffer has DONTFREE flag we need to copy it */
    if( bufp->buf_flags & NG_BUFF_DONTFREE) {
      /* clear busy flag */
      bufp->buf_flags &= ~NG_BUFF_BUSY;
      /* replace with new buffer */
      ngBufAlloc( f_bufp);
      if( f_bufp == NULL) {
        /* cannot create fragments, return */
#ifdef NG_IPSTATS_SUPPORTED
        ngIpStat.ips_odrops++;
#endif
        return( NG_ENOBUFS);
      }
      /* copy the ip header + data that fit in fragment */
      f_bufp->buf_datap = ((NGubyte *) f_bufp) + ngBufDataOffset;
      f_bufp->buf_datalen = bufp->buf_datalen;
      if( f_bufp->buf_datalen > (int) (f_len + hlen)) /* should always be true */
        f_bufp->buf_datalen = f_len + hlen;
      ngMemCpy( f_bufp->buf_datap, bufp->buf_datap, f_bufp->buf_datalen);
      ip = (NGiphdr *) f_bufp->buf_datap;
      bufp = f_bufp;
    }
    else {
      f_bufp = bufp;
    }
    /* allocate message buffers for fragments */
    for( f_off = hlen + f_len; f_off < ip->ip_len; f_off += f_len) {
      tbufp = f_bufp;
      ngBufAlloc( f_bufp);
      if( f_bufp == NULL) {
        /* free all buffers and returns */
        err = NG_ENOBUFS;
#ifdef NG_IPSTATS_SUPPORTED
        ngIpStat.ips_odrops++;
#endif
        goto send_or_free;
      }

      /* chains the buffers */
      tbufp->buf_next = f_bufp;

      /* copy the ip header */
      f_bufp->buf_datap = ((NGubyte *) f_bufp) + ngBufDataOffset;
      /* note: we should not copy all options */
      ngMemCpy( f_bufp->buf_datap, bufp->buf_datap, hlen);
      f_hlen = hlen;
      f_ip = (NGiphdr *) f_bufp->buf_datap;
      /* set the fragment offset */
      f_ip->ip_off = ((f_off - hlen)>>3) + (ip->ip_off & ~IPOFF_MF);
      /* if MF is set on original packet, set on all fragments */
      if( ip->ip_off & IPOFF_MF)
        f_ip->ip_off |= IPOFF_MF;
      /* else set MF on all fragments except last */
      if( f_off + f_len >= ip->ip_len)
        f_len = ip->ip_len - f_off;
      else
        f_ip->ip_off |= IPOFF_MF;
      f_ip->ip_len = ngHTONS( (NGushort) (f_len + f_hlen));

      /* copy the data */
      tlen = bufp->buf_datalen - f_off;
      if( tlen > 0) {
        /* some data from the first buffer */
        ngMemCpy( f_bufp->buf_datap + f_hlen,
                  bufp->buf_datap + f_off,
                  NG_MIN( f_len, (u_int) tlen));
      }
      if( bufp->buf_flags & NG_BUFF_EXTRADATA) {
        tlen = f_off + f_len - f_bufp->buf_datalen;
        if( tlen > 0) {
          /* data are in external iovec buffers */
          ngIovCpy( f_bufp->buf_datap + f_hlen,
                    bufp->buf_iov, bufp->buf_iovcnt,
                    f_off - bufp->buf_datalen,
                    NG_MIN( f_len, (u_int) tlen));
        }
      }

      f_bufp->buf_datalen = f_len + hlen;
      /* set fields in network byte order and calculate header checksum */
      f_ip->ip_off = ngHTONS( f_ip->ip_off);
      f_ip->ip_sum = 0;
      f_ip->ip_sum = ngIpCksum( f_ip, f_hlen, NULL, 0);
    }

    /* copy left data in first fragment */
    if( bufp->buf_flags & NG_BUFF_EXTRADATA) {
      if( bufp->buf_datalen < (int) (hlen + f_firstlen))
        ngIovCpy( bufp->buf_datap + bufp->buf_datalen,
                  bufp->buf_iov, bufp->buf_iovcnt, 0,
                  hlen + f_firstlen - bufp->buf_datalen);
    }
    /* adjust first fragment */
    bufp->buf_flags &= ~(NG_BUFF_RAW|NG_BUFF_EXTRADATA);
    bufp->buf_datalen = hlen + f_firstlen;
    ip->ip_len = ngHTONS( (NGushort) bufp->buf_datalen);
    ip->ip_off = ngHTONS( (NGushort) (ip->ip_off | IPOFF_MF));
    ip->ip_sum = 0;
    ip->ip_sum = ngIpCksum( ip, hlen, NULL, 0);

    /* send all fragments */
    err = 0;
  send_or_free:
    f_bufp = bufp;
    while( f_bufp) {
      tbufp = f_bufp->buf_next;
      if( !err) {
#ifdef NG_DEBUG
        f_ip = (NGiphdr *) f_bufp->buf_datap;
        ngDebug( NG_DBG_IP, NG_DBG_OUTPUT, 0,
          "IPFRAG: %I > %I pr:%d len:%u id:%u off:%d",
          f_ip->ip_src, f_ip->ip_dst, f_ip->ip_p, ngNTOHS( f_ip->ip_len),
          ngNTOHS(f_ip->ip_id), ((u_int)ngNTOHS(f_ip->ip_off)&IPOFF_MASK)<<3 );
#endif
#ifdef NG_IPSTATS_SUPPORTED
        ngIpStat.ips_ofragments++;
#endif
#ifdef NG_IGMP_SUPPORTED
        /* loopback a copy ? */
        if( mloop) {
          NGbuf *loopbufp;
          loopbufp = ngBufCopy( f_bufp);
          if( loopbufp != NULL) ngIpLoopback( out_netp, loopbufp);
        }
#endif
        err = (out_netp->if_output_f)( out_netp, f_bufp, gateway);
      }
      else {
        ngBufOutputFree( f_bufp);
      }
      f_bufp = tbufp;
    }

#ifdef NG_IPSTATS_SUPPORTED
    ngIpStat.ips_fragmented++;
    ngIpStat.ips_opackets++;
#endif

    return( err);
#else
    /* framentation not supported */
    err = NG_EMSGSIZE;
#ifdef NG_IPSTATS_SUPPORTED
    ngIpStat.ips_cantfrag++;
#endif
    goto bad;
#endif
  } else {

    if( bufp->buf_flags & NG_BUFF_EXTRADATA) {
      /* copy extra data into buffer */
#ifdef NG_IPFRAG_SUPPORTED
      ngIovCpy( bufp->buf_datap + bufp->buf_datalen,
                bufp->buf_iov, bufp->buf_iovcnt, 0,
                ip->ip_len - bufp->buf_datalen);
      bufp->buf_datalen = ip->ip_len;
#else
      int i;
      for( i=0; i<bufp->buf_iovcnt; i++) {
        ngMemCpy( bufp->buf_datap + bufp->buf_datalen,
                  bufp->buf_iov[i].iov_base,
                  bufp->buf_iov[i].iov_len);
        bufp->buf_datalen += bufp->buf_iov[i].iov_len;
      }
#endif
    }

    /* set values in net byte order */
    ip->ip_len = ngHTONS( ip->ip_len);
    ip->ip_off = ngHTONS( ip->ip_off);

    /* Finally update checksum (and TTL if IP forwarded enable) */
#ifdef NG_IPFWD_SUPPORTED
    if (
        (ngIp_flags & NG_IPO_FORWARD) &&
        (flags & NG_IPOUTPUT_IPFWD)
    ) {

      NGuint sum;
      NGushort old;

      old = ngNTOHS(*(NGushort*)&ip->ip_ttl);
      ip->ip_ttl--;
      if (!ip->ip_ttl) {
        /* ICMP error TIME EXCEED in transit ... */
        ip->ip_ttl++; /* restore old ttl value for icmp error msg... */
        icmp_type = ICMP_TIMXCEED;
        icmp_code = ICMP_TIMXCEED_INTRANS;
        goto send_icmp;
      } else {
        /* Updating IP checksum (see RFC 1141 & 1624) to reflect ttl change */
        /* Warning: must operate on unsigned long...*/
        sum = (~(ngNTOHS(*(NGushort*)&ip->ip_ttl)) & 0xffff);
        sum += old;
        sum += ngNTOHS(ip->ip_sum);
        sum = (sum & 0xffff) + (sum >> 16);
        ip->ip_sum = ngHTONS( (NGushort) (sum + (sum >> 16)));
#ifdef NG_DEBUG
        if (ngIpCksum( ip, hlen, NULL, 0)) {
          ngDebug(NG_DBG_IP, NG_DBG_OUTPUT, 5,
                  "IPFWD: error invalid IP cksum built! (%x)",
                  ngIpCksum( ip, hlen, NULL, 0));
          err = NG_EINVAL;
          goto bad;
        }
#endif
      }
    } else {
      /* ip header checksum */
      ip->ip_sum = 0;
      ip->ip_sum = ngIpCksum( ip, hlen, NULL, 0);
    }
#else
    /* ip header checksum */
    ip->ip_sum = 0;
    ip->ip_sum = ngIpCksum( ip, hlen, NULL, 0);
#endif

#ifdef NG_IPFWD_SUPPORTED

  send_icmp:

    if (icmp_type) { /* Do not send ICMP packet if type is 0 (hack) */

      switch(icmp_type) {

      case ICMP_UNREACH:
      case ICMP_REDIRECT:
      case ICMP_TIMXCEED:
        /* Sending to same interface of incoming packet */
        out_netp = in_netp;
        if( !(out_netp->if_flags & NG_IFF_UP)) /*lint !e613 (Warning -- Possible use of null pointer) */{
          err = NG_ENETDOWN;
#ifdef NG_IPSTATS_SUPPORTED
          ngIpStat.ips_noroute++;
#endif
          goto bad;
        }
        /* Compute length stuff */
        hlen = (ip->ip_hlv & NG_IPHDRLEN_MASK)<<2; /* Hdr len of original packet */
        dlen = NG_MIN(8, ip->ip_len - hlen); /* Returned data len (up to 8) */
        /* Restore original IP packet */
        ip->ip_len = ngHTONS( ip->ip_len);
        ip->ip_off = ngHTONS( ip->ip_off);
        dest = ip->ip_src;
        /* Now ok to prepare bufp for insertion of new IP & ICMP headers */
        /* Moving up to 8 first bytes of data (skipping whole IP hdr) */
        ngMemMove(
                  (NGubyte*)ip + (sizeof(NGiphdr)*2 + ICMP_MINLEN),
                  (NGubyte*)ip + hlen,
                  dlen );
        /* Move old IP header (w/o any options) in bufp */
        ngMemMove(
                  (NGubyte*)ip + (sizeof(NGiphdr) + ICMP_MINLEN),
                  (NGubyte*)ip,
                  sizeof(NGiphdr) ); /* old IP hdr without option */
        /* Construct new IP header */
        ip->ip_hlv = NG_IPVER_IPV4 | (sizeof(NGiphdr)>>2);
        ip->ip_tos = 0;
        ip->ip_len = ngHTONS( (NGushort)(sizeof(NGiphdr) * 2 +
                                         ICMP_MINLEN + dlen));
        ip->ip_id = ngIpId++;
        ip->ip_id = ngHTONS(ip->ip_id);
        ip->ip_off = 0;
        ip->ip_ttl = ngIp_ttl;
        ip->ip_p = IPPROTO_ICMP;
        /*lint -e613 (Warning -- Possible use of null pointer) */
        ip->ip_src = in_netp->if_addr;
        /*lint +e613 (Warning -- Possible use of null pointer) */
        ip->ip_dst = dest;
        ip->ip_sum = 0;
        ip->ip_sum = ngIpCksum(ip, sizeof(NGiphdr), NULL, 0);
        /* Construct new ICMP header */
        icp = (NGicmp *) ((NGubyte*)ip + sizeof(NGiphdr));
        icp->icmp_type = icmp_type;
        icp->icmp_code = icmp_code;
        if (
            (icmp_type == ICMP_REDIRECT) &&
            (icmp_code == ICMP_REDIRECT_HOST)
        )
          icp->icmp_gwaddr = gateway;
        else
          icp->icmp_void = 0;
        icp->icmp_cksum = 0;
        icp->icmp_cksum = ngIpCksum(icp, ICMP_MINLEN+sizeof(NGiphdr)+dlen,
                                   NULL, 0);
#ifdef NG_DEBUG
        ngDebug(NG_DBG_ICMP, NG_DBG_OUTPUT, 0, "IPFWD: %I > %I %s (%d)",
          ip->ip_src, ip->ip_dst,
          (icp->icmp_type < 19) ?
                icmp_type_str[icp->icmp_type] :
                "Invalid Type",
          icp->icmp_code
        );
        if (ngIpCksum(icp, ICMP_MINLEN+sizeof(NGiphdr)+dlen, NULL, 0)) {
          ngDebug(NG_DBG_ICMP, NG_DBG_OUTPUT, 5,
                  "IPFWD: warning invalid ICMP cksum built (%x)",
                  ngIpCksum(icp, ICMP_MINLEN+sizeof(NGiphdr)+dlen, NULL, 0));
        }
#endif
        /* Setting buffer size */
        bufp->buf_datalen = ngNTOHS(ip->ip_len);
        gateway = dest;
        break;

      default:
#ifdef NG_DEBUG
        ngDebug(NG_DBG_ICMP, NG_DBG_OUTPUT, 5,
                "IPFWD: icmp type = %d not handle", icmp_type);
#endif
        err = NG_EINVAL;
        goto bad;
      }
    } /* end of "if (icmp_type) {" */
#endif

    /* Send the message to the driver */
    bufp->buf_flags &= ~(NG_BUFF_RAW|NG_BUFF_EXTRADATA);
#ifdef NG_DEBUG
    if ( (ip->ip_p == IPPROTO_UDP) || (ip->ip_p == IPPROTO_TCP) ) {
      NGudphdr *uh = (NGudphdr*)((NGubyte*)ip +
                     ((ip->ip_hlv & NG_IPHDRLEN_MASK)<<2));
      ngDebug(NG_DBG_IP, NG_DBG_OUTPUT, 0,
              "%I:%u > %I:%u pr:%d len:%u id:%u off:%d",
              ip->ip_src, ngNTOHS(uh->uh_sport),
              ip->ip_dst, ngNTOHS(uh->uh_dport),
              ip->ip_p, ngNTOHS(ip->ip_len),
              ngNTOHS(ip->ip_id), ((u_int)ip->ip_off&IPOFF_MASK)<<3 );
    } else
      ngDebug( NG_DBG_IP, NG_DBG_OUTPUT, 0,
        "%I > %I pr:%d len:%u id:%u off:%d",
        ip->ip_src, ip->ip_dst, ip->ip_p, ngNTOHS( ip->ip_len),
        ngNTOHS(ip->ip_id), ((u_int)ngNTOHS( ip->ip_off)&IPOFF_MASK)<<3 );
#endif
#ifdef NG_IPSTATS_SUPPORTED
    if( flags & NG_IPOUTPUT_IPFWD) {
      ngIpStat.ips_forward++;
    }
    else {
      ngIpStat.ips_opackets++;
    }
#endif
#ifdef NG_IGMP_SUPPORTED
    /* loopback a copy ? */
    if( mloop) {
      NGbuf *loopbufp;
      loopbufp = ngBufCopy( bufp);
      if( loopbufp != NULL) ngIpLoopback( out_netp, loopbufp);
    }
#endif
    /*lint -e613 (Warning -- Possible use of null pointer) */
    err = (out_netp->if_output_f)( out_netp, bufp, gateway);
    /*lint +e613 (Warning -- Possible use of null pointer) */
    return (err);
  }

 bad:
  ngBufOutputFree( bufp);
#ifdef NG_DEBUG
  ngDebug( NG_DBG_IP, NG_DBG_OUTPUT, 1, "Packet discarded (%d)", err);
#endif
  return( err);
}

