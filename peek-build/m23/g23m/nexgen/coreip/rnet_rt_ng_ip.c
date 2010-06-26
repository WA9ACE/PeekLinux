/*****************************************************************************
 * $Id: ip.c,v 1.7.2.1 2002/10/28 18:49:49 rf Exp $
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
 * ipInit()
 * ipCntl()
 * ipTimer()
 * ipInput()
 * ngProto_IP
 *----------------------------------------------------------------------------
 * 22/09/98 - Regis Feneon
 * 27/11/98 -
 *  New protocol structure integration
 * 15/12/98 -
 *  Icmp time-stamp message correction
 * 29/12/98
 *  global variable names changed
 *  new function ipCntl() added
 * 12/01/99 -
 *  now accept all datagrams if interface is not configured
 *  (bootp and dhcp issues)
 * 01/02/99 -
 *  test cksum on incoming icmp messages
 * 05/02/99 -
 *  full fragmentation support added
 * 25/04/99 -
 *  routing options corrected
 * 12/07/99 -
 * 10/02/2000 - ipInput()
 *  test for ip header alignment
 * 18/02/2000 - Adrien Felon
 *  adding argument "flag" for ngIpOutput() call in ipInput()
 *  adding code for IP fwd support
 *  beeing more verbose in debug info at level 0 (IP module)
 * 07/04/2000 -
 *  addind code for NAT fwd support
 *  changing name of global var "icmp_type" to "icmp_type_str"
 *  ipInput(): dst/src address swap with ICMP msgs (no more using INADDR_ANY)
 * 14/04/2000 -
 *  - renaming NAT configuration options to use new IP config ones
 *  - add call to ngNatInit() in ipInit()
 * 31/05/2000 -
 *  corrected buffer alignment test
 * 09/06/2000 -
 *  added static data initialization
 * 03/07/2000 -
 *  corrected correction of buffer alignment test (!)
 * 19/09/2000 -
 *  ipCntl() - clear route table when setting NG_IPO_ROUTE_TABLE
 * 13/10/2000 - Adrien Felon
 *  removed NG_IPO_NAT_ADDEXTIF
 *  added NG_IPO_NAT_IFPUB NG_IPO_NAT_IFPUB
 * 02/11/2000 - Regis Feneon
 *  added IGMP support
 *  added ngIp_random variable
 * 16/11/2000 -
 *  added IP statistics
 *  corrected address swapping for icmp replies
 * 24/11/2000 -
 *  new ngProtoList & NGproto structure
 *  removed ngIpProtoTable[]
 * 23/01/2001 -
 *  corrections in NGiphdr structure
 * 06/03/2001 -
 *  added NG_IPO_NETDOWN option
 * 09/04/2002 -
 *  corrected reassembly code (buf_datalen setting of first chained buffer)
 * 28/10/2002 -
 *  ipCntl: added support for NG_CNTL_GET
 *****************************************************************************/

#include <ngip.h>
#include <ngip/ip.h>
#include <ngip/icmp.h>
#include <ngip/igmp.h>
#include <ngip/udp.h>
#include <ngtcp.h>
#include "rvf_target.h"

#ifdef NG_NATFWD_SUPPORTED
#include <ngnat.h>
#endif

#ifdef NG_IPFRAG_SUPPORTED
u_int ngIp_frag_timeo; /* reassembly timeout (60 sec) */
static NGipfrag ipf[NG_IPFRAG_NBMAX]; /* fragments table */
#endif

#ifdef NG_IGMP_SUPPORTED
static int inmulti_max;
static void *inmulti_table;

NGmemblk ngIp_multi_free; /* free NGinmulti structures */
int ngIp_multi_timers; /* REPORT timers are pending */
#endif

#ifdef NG_DEBUG
const char *icmp_type_str[] = {
  "Echo Reply",
  "Invalid Type 1",
  "Invalid Type 2",
  "Dest. Unreachable",
  "Source Quench",
  "Redirection",
  "Invalid Type 6",
  "Invalid Type 7",
  "Echo Request",
  "Router Advertisement",
  "Router Solicitation",
  "Time Exceeded",
  "Parameter Problem",
  "Timestamp Request",
  "Timestamp Reply",
  "Information Request",
  "Information Reply",
  "Address Mask Request",
  "Address Mask Reply"
};
#endif

/*****************************************************************************
 * ipInit()
 *****************************************************************************
 * Initialize IP layer
 *****************************************************************************
 * Parameters: none
 * Return value: none
 */

static void ipInit( void)
{
#ifdef NG_IPFRAG_SUPPORTED
  int i;
#endif
  /* initial option values */
  ngIp_ttl = 64;
  ngIp_tos = 0;
  ngIp_flags = NG_UDPO_CHECKSUM & NG_IPO_FLAGSMASK;

  /* Dtagrams Id */
  ngIpId = 1;

  /* random generator */
  ngIp_random = (NGuint) ngOSClockGetTime();

  /* routing */
  ngIp_route_max = 0;
  ngIp_route_default.rt_ifnetp = NULL;
  ngIp_route_default.rt_gateway = 0;
  ngIp_route_default.rt_addr = 0;
  ngIp_route_default.rt_subnetmask = 0;
#ifdef NG_IPFRAG_SUPPORTED
  ngIp_frag_timeo = 60*NG_TIMERHZ;
  /* initialize fragment table */
  for( i=0; i<NG_IPFRAG_NBMAX; i++)
    ipf[i].ipf_ttl = 0;
#endif
#ifdef NG_IGMP_SUPPORTED
  NG_MEMBLK_INIT( &ngIp_multi_free);
  ngIp_multi_timers = 0;
  inmulti_max = 0;
  inmulti_table = NULL;
#endif
#ifdef NG_IPSTATS_SUPPORTED
  ngMemSet( &ngIpStat, 0, sizeof( ngIpStat));
  ngMemSet( &ngIcmpStat, 0, sizeof( ngIcmpStat));
#ifdef NG_IGMP_SUPPORTED
  ngMemSet( &ngIgmpStat, 0, sizeof( ngIgmpStat));
#endif
#endif
#ifdef NG_NATFWD_SUPPORTED
  ngNatInit();
#endif
  return;
}

/*****************************************************************************
 * ipCntl()
 *****************************************************************************
 * Protocol control function
 *****************************************************************************
 * Parameters:
 * Return value: 0 or error code
 */

static int ipCntl( int cmd, int opt, void *arg)
{
#ifdef NG_NATFWD_SUPPORTED
  NGifnet *netp;
#endif
  NGiprtent *rtp;
  int i;

  if( cmd == NG_CNTL_SET) switch( opt) {

  case NG_IPO_TTL:
    ngIp_ttl = *((int *) arg);
    break;

  case NG_IPO_TOS:
    ngIp_tos = *((int *) arg);
    break;

  case NG_IPO_FORWARD:
  case NG_IPO_SENDREDIRECT:
  case NG_ICMPO_MASKREPLY:
  case NG_UDPO_CHECKSUM:
    if( *((int *) arg))
      ngIp_flags |= (opt & NG_IPO_FLAGSMASK);
    else
      ngIp_flags &= ~(opt & NG_IPO_FLAGSMASK);
    break;

#ifdef NG_IPFRAG_TIMEO
  case NG_IPO_FRAG_TIMEO:
    ngIp_frag_timeo = *((u_int *) arg);
    break;
#endif

  case NG_IPO_ROUTE_DEFAULT:
    ngIp_route_default.rt_gateway = *((NGuint *) arg);
    ngIpRouteUpdate();
    break;
  case NG_IPO_ROUTE_MAX:
    if( ngIp_route_max) return( NG_EINVAL);
    ngIp_route_max = *((int *) arg);
    break;
  case NG_IPO_ROUTE_TABLE:
    if( !ngIp_route_max) return( NG_EINVAL);
    ngIp_route_table = *((NGiprtent **) arg);
    /* clear route table */
    ngMemSet( ngIp_route_table, 0, ngIp_route_max*sizeof( NGiprtent));
    break;
  case NG_IPO_ROUTE_ADD:
  case NG_IPO_ROUTE_DELETE:
    if( (ngIp_route_max == 0) || (ngIp_route_table == NULL))
      return( NG_EINVAL);
    if( opt == NG_IPO_ROUTE_ADD) {
      /* route already exist ? */
      rtp = NULL;
      for( i=0; i<ngIp_route_max; i++) {
        if( ngIp_route_table[i].rt_addr ==
            ((NGiprtent *) arg)->rt_addr) {
          rtp = &ngIp_route_table[i];
          break;
        }
      }
      if( rtp == NULL) {
        /* find an empty entry */
        for( i=0; i<ngIp_route_max; i++) {
          if( ngIp_route_table[i].rt_gateway == INADDR_ANY) {
            rtp = &ngIp_route_table[i];
            break;
          }
        }
      }
      if( rtp == NULL) return( NG_ENOSPC);
      rtp->rt_gateway = ((NGiprtent *) arg)->rt_gateway;
      rtp->rt_addr = ((NGiprtent *) arg)->rt_addr;
      rtp->rt_subnetmask = ((NGiprtent *) arg)->rt_subnetmask;
      ngIpRouteUpdate();
    }
    else {
      /* find the entry */
      for( i=0; i<ngIp_route_max; i++) {
        if( ngIp_route_table[i].rt_addr == *((NGuint *) arg)) {
          /* delete entry */
          ngIp_route_table[i].rt_gateway = INADDR_ANY;
          break;
        }
      }
    }
    break;

#ifdef NG_NATFWD_SUPPORTED
  case NG_IPO_NAT_TABLEMAX:
    ngNAT_vars.table_size = *((int*)arg);
    return( NG_EOK);
  case NG_IPO_NAT_TABLE:
    ngNAT_vars.table = *((NGnatent**)arg);
    return( NG_EOK);
  case NG_IPO_NAT_ALGMAX:
    ngNAT_vars.alg_size = *((int*)arg);
    return( NG_EOK);
  case NG_IPO_NAT_ALG:
    ngNAT_vars.alg = *((NGnatalg**)arg);
    return( NG_EOK);
  case NG_IPO_NAT_IFPUB:
    netp = *((NGifnet**)arg);
    netp->if_flags |= NG_IFF_EXTNAT;
    return( NG_EOK);
  case NG_IPO_NAT_IFPRIV:
    netp = *((NGifnet**)arg);
    netp->if_flags &= ~NG_IFF_EXTNAT;
    return( NG_EOK);
  case NG_IPO_NAT_TO_TCP:
    ngNAT_vars.to_tcp = *((long*)arg);
    return( NG_EOK);
  case NG_IPO_NAT_TO_TCP_FIN:
    ngNAT_vars.to_tcp_fin = *((long*)arg);
    return( NG_EOK);
  case NG_IPO_NAT_TO_UDP:
    ngNAT_vars.to_udp = *((long*)arg);
    return( NG_EOK);
  case NG_IPO_NAT_TO_ICMP:
    ngNAT_vars.to_icmp = *((long*)arg);
    return( NG_EOK);
#endif

#ifdef NG_IGMP_SUPPORTED
  case NG_IPO_INMULTI_MAX:
  case NG_IPO_INMULTI_TABLE:
    if( opt == NG_IPO_INMULTI_MAX) {
      if( inmulti_max) return( NG_EINVAL);
      inmulti_max = *((int*)arg);
    }
    else {
      if( inmulti_table) return( NG_EINVAL);
      inmulti_table = *((void **)arg);
    }
    if( inmulti_max && inmulti_table) {
      /* initialize mem block */
      ngMemBlkInit( &ngIp_multi_free,
        inmulti_table, inmulti_max, sizeof( NGinmulti));
    }
    break;
#endif

  case NG_IPO_NETDOWN:
    /* let tcp know that some connections must be closed */
    if( ngProtoList[NG_PROTO_TCP] != NULL)
      (void)(ngProtoList[NG_PROTO_TCP]->pr_cntl_f( NG_CNTL_SET,
        NG_TCPO_ROUTE_UPDATE, NULL));
    break;

  default:
    return( NG_EINVAL);
  }
  else if( cmd == NG_CNTL_GET) switch( opt) {

  case NG_IPO_TTL:
    *((int *) arg) = ngIp_ttl;
    break;

  case NG_IPO_TOS:
    *((int *) arg) = ngIp_tos;
    break;

  case NG_IPO_FORWARD:
  case NG_IPO_SENDREDIRECT:
  case NG_ICMPO_MASKREPLY:
  case NG_UDPO_CHECKSUM:
    *((int *) arg) = ngIp_flags & (opt & NG_IPO_FLAGSMASK);
    break;

#ifdef NG_IPFRAG_TIMEO
  case NG_IPO_FRAG_TIMEO:
    *((u_int *) arg) = ngIp_frag_timeo;
    break;
#endif

  default:
    return( NG_EINVAL);
  }
  else {
    return( NG_EINVAL);
  }
  return( NG_EOK);
}

/*****************************************************************************
 * ipTimer()
 *****************************************************************************
 * IP fragmentation time-out reports
 * IGMP reports
 *****************************************************************************
 * Parameters: none
 * Return value: none
 */

#if defined(NG_IPFRAG_SUPPORTED) || defined(NG_NATFWD_SUPPORTED) \
  || defined(NG_IGMP_SUPPORTED)

static void ipTimer( void)
{
  int i;
  NGbuf *bufp;
#ifdef NG_IGMP_SUPPORTED
  NGifnet *netp;
  NGinmulti *inmp;
#endif

#ifdef NG_NATFWD_SUPPORTED
  ngNatTimer();
#endif

#ifdef NG_IPFRAG_SUPPORTED
  /* fragment timeout */
  for( i=0; i<NG_IPFRAG_NBMAX; i++) {
    if( ipf[i].ipf_ttl && (--ipf[i].ipf_ttl == 0)) {
      /* remove the entry */
      while( 1) {
        NG_NODE_OUT( &ipf[i].ipf_node, bufp);
        if( bufp == NULL) break;
        ngBufFree( bufp);
      }
      ipf[i].ipf_ttl = 0;
#ifdef NG_IPSTATS_SUPPORTED
      ngIpStat.ips_reassfailed++;
#endif
    }
  }
#endif

#ifdef NG_IGMP_SUPPORTED
  /* timer pending ? */
  if( ngIp_multi_timers) {
    ngIp_multi_timers = 0;

    /* parse list of interfaces */
    netp = ngIfList;
    while( netp) {
      /* parse list of multi groups */
      inmp = netp->if_multiaddrs;
      while( inmp) {
        if( inmp->inm_timer) {
          if( --inmp->inm_timer == 0) {
            /* timer has expired, send report msg */
            ngIgmpSendReport( inmp);
          }
          else ngIp_multi_timers = 1; /* at least one timer still active */
        }
        /* next address */
        inmp = inmp->inm_next;
      }
      /* next interface */
      netp = netp->if_next;
    }
  }
#endif

  return;
}
#else
#define ipTimer NULL
#endif

/*****************************************************************************
 * ipInput()
 *****************************************************************************
 *  Reception of ip messages
 *  icmp messages are directly treated
 *  other messages are passed to the transport levels
 *****************************************************************************
 * Parameters:
 *  bufp    input message buffer
 * Return value: none
 */

static void ipInput( NGbuf *bufp)
{
  NGifnet *netp, *net2p;
  NGiphdr *ip;
  NGicmp *icp;
  int hlen, i, code;
#ifdef NG_IPFRAG_SUPPORTED
  int olen;
  NGbuf *tbufp, *f_bufp;
#endif
  NGuint tmpaddr;
#ifdef NG_IGMP_SUPPORTED
  NGigmp *igp;
  NGinmulti *inmp;
#endif

#ifdef NG_IPSTATS_SUPPORTED
  ngIpStat.ips_ipackets++;
#endif

  /* get network interface */
  netp = (NGifnet *) bufp->buf_ifnetp;

#ifdef NG_ALIGN_STRICT
  /* align ip header on word boundary */
  if( bufp->buf_flags & NG_BUFF_UNALIGNED) {
    int delta;
#if defined (NG_ALIGN16)
    delta = (int) ((unsigned long) bufp->buf_datap & 1UL);
#elif defined (NG_ALIGN32)
    delta = (int) ((unsigned long) bufp->buf_datap & 3UL);
#else
#error "NG_ALIGNXX macro not valid"
#endif
    ngMemMove( bufp->buf_datap - delta, bufp->buf_datap, bufp->buf_datalen);
    bufp->buf_datap -= delta;
    bufp->buf_flags &= ~NG_BUFF_UNALIGNED;
  }
#endif

  ip = (NGiphdr *) bufp->buf_datap;

  /* if( netp->if_addr == 0) goto bad; */

  /* ip version */
  if( (ip->ip_hlv & NG_IPVER_MASK) != NG_IPVER_IPV4) {
    goto bad;
  }

  /* header size */
  hlen = (ip->ip_hlv & NG_IPHDRLEN_MASK)<<2;
  if( (unsigned)hlen < sizeof( NGiphdr)) goto bad;

  /* checksum */
  if( ngIpCksum( bufp->buf_datap, hlen, NULL, 0) != 0)
  {
    goto bad;
  }

  /* host byte order */
  ip->ip_len = ngNTOHS( ip->ip_len);
  ip->ip_off = ngNTOHS( ip->ip_off);
  /* size of message */
  if( ip->ip_len < hlen) goto bad;
  if( bufp->buf_datalen < ip->ip_len) goto bad;
  if( bufp->buf_datalen > ip->ip_len) bufp->buf_datalen = ip->ip_len;

  /* do ip options... NOT SUPPORTED */

  /* which destination ? */

#ifdef NG_DEBUG
  if ( (ip->ip_p == IPPROTO_UDP) || (ip->ip_p == IPPROTO_TCP) ) {
    NGudphdr * uh = (NGudphdr*)((NGubyte*)ip + hlen);
    ngDebug(NG_DBG_IP, NG_DBG_INPUT, 0,
            "%I:%u < %I:%u pr:%d len:%u id:%u off:%d %c",
            ip->ip_dst, ngNTOHS(uh->uh_dport),
            ip->ip_src, ngNTOHS(uh->uh_sport),
            ip->ip_p, ip->ip_len,
            ngNTOHS(ip->ip_id),
            ((u_int)ip->ip_off&IPOFF_MASK)<<3,
             (bufp->buf_flags & NG_BUFF_BCAST) ? 'B' :
            ((bufp->buf_flags & NG_BUFF_MCAST) ? 'M' : ' ') );
  } else
    ngDebug(NG_DBG_IP, NG_DBG_INPUT, 0,
            "%I < %I pr:%d len:%u id:%u off:%d %c",
            ip->ip_dst, ip->ip_src, ip->ip_p, ip->ip_len,
            ngNTOHS(ip->ip_id),
            ((u_int)ip->ip_off&IPOFF_MASK)<<3,
             (bufp->buf_flags & NG_BUFF_BCAST) ? 'B' :
            ((bufp->buf_flags & NG_BUFF_MCAST) ? 'M' : ' ') );
#endif

  /* if interface is not configured, accept all datagrams */
  /* this is for bootp/dhcp configurations */
  if( netp->if_addr == 0) goto ours;

  /* look for an address match whith any interface */
  net2p = ngIfList;
  while( net2p != NULL) {
    if( ( (net2p->if_flags & (NG_IFF_UP|NG_IFF_RUNNING))
          == (NG_IFF_UP|NG_IFF_RUNNING)) &&
        (net2p->if_addr == ip->ip_dst)
    ) {
#ifdef NG_NATFWD_SUPPORTED
      /* We do not want one of our private interface to be
       * reached from external network...
       */
      if (
          (netp->if_flags & NG_IFF_EXTNAT) &&
          !(net2p->if_flags & NG_IFF_EXTNAT)
      ) {
#ifdef NG_DEBUG
        ngDebug(NG_DBG_NAT, NG_DBG_INPUT, 0,
                "Discarding packet from ext trying to access private net if");
#endif
        goto bad;
      }
      /* Ok to call NAT input function (will check for a masqueraded host) */
      if (
          (netp->if_flags & NG_IFF_EXTNAT) &&
          (net2p->if_flags & NG_IFF_EXTNAT)
      ) {
        if (NG_EOK == ngNatInput(bufp))
          return;
        /* else: try to handle packet localy (not forwarding) goto ours... */
      }
#endif /* NATFWD defined */
      goto ours;
    }
    net2p = net2p->if_next;
  }
  /* try all broadcast address */
  if( ip->ip_dst == netp->if_broadaddr) goto ours;
  if( ip->ip_dst == netp->if_netbroadcast) goto ours;
  if( ip->ip_dst == NG_INADDR_BROADCAST) goto ours;
  /* old broadcast address */
  if( ip->ip_dst == netp->if_subnet) goto ours;
  if( ip->ip_dst == netp->if_net) goto ours;
  if( ip->ip_dst == NG_INADDR_ANY) goto ours;

  /* multicast address ? */
  if( NG_IN_MULTICAST( ip->ip_dst)) {
#ifdef NG_IGMP_SUPPORTED
    /* always accept all groups address */
    if( ip->ip_dst == NG_INADDR_ALLHOSTS_GROUP) goto ours;
    /* look for the interface group */
    inmp = netp->if_multiaddrs;
    while( inmp) {
      if( inmp->inm_addr == ip->ip_dst) goto ours;
      inmp = inmp->inm_next;
    }
#endif
    /* not supported, discard! */
#ifdef NG_IPSTATS_SUPPORTED
    ngIpStat.ips_cantforward++;
#endif
    goto drop;
  }

  /* not for us... forward ? */
#ifdef NG_IPFWD_SUPPORTED
  if( ngIp_flags & NG_IPO_FORWARD) {
    ngIpOutput(bufp, NG_IPOUTPUT_IPFWD);
    return;
  }
#endif /* IPFWD */
#ifdef NG_IPSTATS_SUPPORTED
  ngIpStat.ips_cantforward++;
#endif
  goto drop;

 ours:
  if( ip->ip_off & (IPOFF_MASK|IPOFF_MF)) {
#ifdef NG_IPSTATS_SUPPORTED
    ngIpStat.ips_ifragments++;
#endif
    /*
     * Fragment received
     */
#ifdef NG_IPFRAG_SUPPORTED
    /* look for a existing entry */
    for( i=0; i<NG_IPFRAG_NBMAX; i++) {
      if( ipf[i].ipf_ttl &&
           (ipf[i].ipf_id == ip->ip_id) &&
           (ipf[i].ipf_src == ip->ip_src) &&
           (ipf[i].ipf_dst == ip->ip_dst) &&
           (ipf[i].ipf_p == ip->ip_p)) break;
    }
    /* eventually find a empty entry */
    if( i >= NG_IPFRAG_NBMAX) for( i=0; i<NG_IPFRAG_NBMAX; i++) {
      if( ipf[i].ipf_ttl == 0) {
        /* initialize the new entry */
        ipf[i].ipf_node.next = ipf[i].ipf_node.prev = &ipf[i].ipf_node;
        ipf[i].ipf_id = ip->ip_id;
        ipf[i].ipf_p = ip->ip_p;
        ipf[i].ipf_src = ip->ip_src;
        ipf[i].ipf_dst = ip->ip_dst;
        ipf[i].ipf_ttl = ngIp_frag_timeo;
        ipf[i].ipf_hlen = 0;
        break;
      }
    }
    if( i >= NG_IPFRAG_NBMAX) {
      goto drop;
    }

    /* save MF flag */
    if( ip->ip_off & IPOFF_MF) bufp->buf_flags |= NG_BUFF_MOREDATA;
    else bufp->buf_flags &= ~NG_BUFF_MOREDATA;

    /* data offset */
    bufp->buf_iovcnt = (ip->ip_off & IPOFF_MASK)<<3;
    if( bufp->buf_iovcnt == 0) {
      /* save ip header length */
      ipf[i].ipf_hlen = hlen;
    }
    /* adjust buffer pointers to ip data only */
    bufp->buf_datap += hlen;
    bufp->buf_datalen -= hlen;

    /* test for maximum datagram size */
    if( bufp->buf_datalen + bufp->buf_iovcnt > NG_IPFRAG_SIZEMAX) {
      /* the datagram is too big */
      goto drop;
    }

    /* find the fragment following the new one */
    tbufp = (NGbuf *) ipf[i].ipf_node.next;
    while( tbufp != (NGbuf *) &ipf[i].ipf_node) {
      if( tbufp->buf_iovcnt > bufp->buf_iovcnt) break;
      tbufp = (NGbuf *) tbufp->buf_node.next;
    }

    /* discard data overlapping previous fragment */
    if( tbufp->buf_node.prev != &ipf[i].ipf_node) {
      olen = ((NGbuf *) tbufp->buf_node.prev)->buf_iovcnt +
        ((NGbuf *) tbufp->buf_node.prev)->buf_datalen -
        bufp->buf_iovcnt;
      if( olen > 0) {
        if( olen >= bufp->buf_datalen) {
          /* discard all buffer */
          goto drop;
        }
        /* adjust pointer */
        bufp->buf_datap += olen;
        bufp->buf_iovcnt += olen;
        bufp->buf_datalen -= olen;
      }
    }

    /* discard data of next overlapped fragments */
    while(
          (tbufp != (NGbuf *) &ipf[i].ipf_node) &&
          (
           (olen = bufp->buf_iovcnt + bufp->buf_datalen - tbufp->buf_iovcnt)
           > 0)
    ) {
      if( olen < tbufp->buf_datalen) {
        /* adjust fragment pointer */
        tbufp->buf_datap += olen;
        tbufp->buf_iovcnt += olen;
        tbufp->buf_datalen -= olen;
        break;
      }
      /* discard all fragment */
      f_bufp = (NGbuf *) tbufp->buf_node.next;
      NG_NODE_DETACH( tbufp);
      ngBufFree( tbufp);
      tbufp = f_bufp;
    }

    /* insert new fragment in the list */
    NG_NODE_IN( tbufp, bufp);

    /* test for complete datagram */
    tbufp = (NGbuf *) ipf[i].ipf_node.next;
    olen = 0;
    while( tbufp != (NGbuf *) &ipf[i].ipf_node) {
      if( tbufp->buf_iovcnt != olen) {
        return;
      }
      olen += tbufp->buf_datalen;
      tbufp = (NGbuf *) tbufp->buf_node.next;
    }
    if( ((NGbuf *) tbufp->buf_node.prev)->buf_flags & NG_BUFF_MOREDATA) {
      return;
    }

    /* we have received the complete datagram */
#ifdef NG_IPSTATS_SUPPORTED
    ngIpStat.ips_reassembled++;
#endif

    /* get the first fragment */
    NG_NODE_OUT( &ipf[i].ipf_node, bufp);

    /* To avoid lint warning, as there is chance of NULL in above macro */
    if( bufp == NULL )
    {
      return;
    }

    /* ip header */
    bufp->buf_datap -= ipf[i].ipf_hlen;
    bufp->buf_datalen += ipf[i].ipf_hlen;
    ip = (NGiphdr *) bufp->buf_datap;
    ip->ip_len = olen + ipf[i].ipf_hlen;

    /* construct the chain of message buffers */
    tbufp = bufp;
    while( 1) {
      NG_NODE_OUT( &ipf[i].ipf_node, f_bufp);
      if( f_bufp && (tbufp == bufp)) {
        /* move maximum data into first buffer */
        olen = ngBufDataMax - bufp->buf_datalen;
        if( olen > f_bufp->buf_datalen) olen = f_bufp->buf_datalen;
        if( olen > 0) {
          ngMemCpy( bufp->buf_datap + bufp->buf_datalen,
                    f_bufp->buf_datap, olen);
          bufp->buf_datalen += olen;
          if( olen == f_bufp->buf_datalen) {
            ngBufFree( f_bufp);
            continue;
          }
          f_bufp->buf_datap += olen;
          f_bufp->buf_datalen -= olen;
        }
      }
      tbufp->buf_next = f_bufp;
      if( f_bufp) {
        tbufp->buf_flags |= NG_BUFF_MOREDATA;
      }
      else {
        tbufp->buf_flags &= ~NG_BUFF_MOREDATA;
        break;
      }
      tbufp = f_bufp;
    }

    /* free the ipf entry */
    ipf[i].ipf_ttl = 0;
#else
    /* fragmentation not supported */
#ifdef NG_IPSTATS_SUPPORTED
    ngIpStat.ips_reassfailed++;
#endif
    goto drop;
#endif
  }

  /*
   * Transmits datagram to upper level protocol
   */

#ifdef NG_IPSTATS_SUPPORTED
  ngIpStat.ips_delivered++;
#endif

  switch( ip->ip_p) {
  case IPPROTO_UDP:
    if( ngProtoList[NG_PROTO_UDP]) {
      (ngProtoList[NG_PROTO_UDP]->pr_input_f)( bufp);
      return;
    }
    goto send_rawip;

  case IPPROTO_TCP:
    if( ngProtoList[NG_PROTO_TCP]) {
      (ngProtoList[NG_PROTO_TCP]->pr_input_f)( bufp);
      return;
    }
    goto send_rawip;

#ifdef NG_IGMP_SUPPORTED
    /*
     * IGMP protocol
     */
  case IPPROTO_IGMP:

#ifdef NG_IPSTATS_SUPPORTED
    ngIgmpStat.igps_ipackets++;
#endif

#ifdef NG_IPFRAG_SUPPORTED
    /* igmp fragmented data is not supported */
    if( bufp->buf_flags & NG_BUFF_MOREDATA) {
      goto send_rawip;
    }
#endif

    /* igmp data */
    igp = (NGigmp *) (bufp->buf_datap + hlen);

    /* validate length */
    if( (ip->ip_len - hlen) < IGMP_MINLEN) {
#ifdef NG_IPSTATS_SUPPORTED
      ngIgmpStat.igps_ierrors++;
#endif
      goto drop;
    }

    /* validate checksum */
    if( ngIpCksum( igp, ip->ip_len - hlen, NULL, 0)) {
#ifdef NG_IPSTATS_SUPPORTED
      ngIgmpStat.igps_ierrors++;
#endif
      goto drop;
    }

#ifdef NG_DEBUG
    ngDebug( NG_DBG_ICMP, NG_DBG_INPUT, 0, "%I < %I %02X %I",
             ip->ip_dst, ip->ip_src, igp->igmp_type, igp->igmp_group);
#endif

    switch( igp->igmp_type) {
    case IGMP_HOST_MEMBERSHIP_QUERY:
      /* queries must be addressed to all-hosts */
      if( ip->ip_dst != NG_INADDR_ALLHOSTS_GROUP) {
#ifdef NG_IPSTATS_SUPPORTED
        ngIgmpStat.igps_ierrors++;
#endif
        goto drop;
      }
#ifdef NG_IPSTATS_SUPPORTED
      ngIgmpStat.igps_iqueries++;
#endif
      /* arm report timers for this interface */
      inmp = netp->if_multiaddrs;
      while( inmp) {
        if( inmp->inm_timer == 0) {
          inmp->inm_timer = ( ((ngRandom( &ngIp_random) & 0xff) *
            IGMP_MAX_HOST_REPORT_DELAY*NG_TIMERHZ) >> 8) + 1;
          ngIp_multi_timers = 1;
        }
        inmp = inmp->inm_next;
      }
      break;
    case IGMP_HOST_MEMBERSHIP_REPORT:
#ifdef NG_IPSTATS_SUPPORTED
      ngIgmpStat.igps_ireports++;
#endif
      /* look if we have a timer armed for this group */
      inmp = netp->if_multiaddrs;
      while( inmp) {
        if( inmp->inm_addr == igp->igmp_group) {
#ifdef NG_IPSTATS_SUPPORTED
          ngIgmpStat.igps_iourreports++;
#endif
          /* stop the timer */
          inmp->inm_timer = 0;
          break;
        }
        inmp = inmp->inm_next;
      }
      break;
    }
    goto send_rawip; /* pass valid igmp messages to rawip layer */
#endif

    /*
     * ICMP Protocol
     */
  case IPPROTO_ICMP:

#ifdef NG_IPSTATS_SUPPORTED
    ngIcmpStat.icps_ipackets++;
#endif

#ifdef NG_IPFRAG_SUPPORTED
    /* icmp fragmented data is not supported */
    if( bufp->buf_flags & NG_BUFF_MOREDATA) {
      goto send_rawip;
    }
#endif

    /* pointer to icmp data */
    icp = (NGicmp *) (bufp->buf_datap + hlen);

#ifdef NG_DEBUG
    ngDebug( NG_DBG_ICMP, NG_DBG_INPUT, 0, "%I < %I %s (%d)",
             ip->ip_dst, ip->ip_src,
             icp->icmp_type < sizeof(icmp_type_str)/sizeof( icmp_type_str[0]) ?
             icmp_type_str[icp->icmp_type] : "Invalid Type", icp->icmp_code);
#endif

    /* minimum tests for valid icmp */
    if( (ip->ip_len - hlen) < ICMP_MINLEN) {
#ifdef NG_IPSTATS_SUPPORTED
      ngIcmpStat.icps_ierrors++;
#endif
      goto drop;
    }
    /* test icmp checksum ? */
    if( ngIpCksum( icp, ip->ip_len - hlen, NULL, 0)) {
#ifdef NG_IPSTATS_SUPPORTED
      ngIcmpStat.icps_ierrors++;
#endif
      goto drop;
    }
    /* type of message */
    code = icp->icmp_code;
#ifdef NG_IPSTATS_SUPPORTED
    if( icp->icmp_type < sizeof( ngIcmpStat.icps_inhist)/
                         sizeof( ngIcmpStat.icps_inhist[0]) ) {
      ngIcmpStat.icps_inhist[icp->icmp_type]++;
    }
#endif
    switch( icp->icmp_type) {
      /* error processing */
    case ICMP_UNREACH:
      switch( code) {
      case ICMP_UNREACH_NET:
      case ICMP_UNREACH_HOST:
      case ICMP_UNREACH_PROTOCOL:
      case ICMP_UNREACH_PORT:
      case ICMP_UNREACH_SRCFAIL:
        code += NG_PRC_UNREACH_NET;
        goto icmp_deliver;
      case ICMP_UNREACH_NEEDFRAG:
        code = NG_PRC_MSGSIZE;
        goto icmp_deliver;
      }
      goto icmp_badcode;

    case ICMP_TIMXCEED:
      code = NG_PRC_TIMXCEED;
      goto icmp_deliver;

    case ICMP_PARAMPROB:
      code = NG_PRC_PARAMPROB;
      goto icmp_deliver;

    case ICMP_SOURCEQUENCH:
      code = NG_PRC_QUENCH;
      /* advise transport protocols */
    icmp_deliver:
      switch( icp->icmp_ip.ip_p) {
      case IPPROTO_UDP:
        i = NG_PROTO_UDP;
        break;
      case IPPROTO_TCP:
        i = NG_PROTO_TCP;
        break;
      default:
        i = NG_PROTO_RAWIP;
      }
      if( ngProtoList[i]) {
        if( ngProtoList[i]->pr_error_f) {
          ((void (*)( int, NGuint, NGiphdr *))
           (ngProtoList[i]->pr_error_f))( code, ip->ip_src, &icp->icmp_ip);
          ngBufFree( bufp);
          return;
        }
      }
      break;

    icmp_badcode:
      /* stats... */
#ifdef NG_IPSTATS_SUPPORTED
      ngIcmpStat.icps_ierrors++;
#endif
      break;

    case ICMP_ECHO:
      icp->icmp_type = ICMP_ECHOREPLY;
      goto icmp_reflect;

    case ICMP_TSTAMP:
      icp->icmp_type = ICMP_TSTAMPREPLY;
      /* timetsamp is non-standart, set bit 31 ! */
      icp->icmp_rtime = (ngOSClockGetTime()*1000UL/ngOSClockGetFreq())
        | 0x80000000UL;
      icp->icmp_rtime = ngHTONL( icp->icmp_rtime);
      icp->icmp_ttime = icp->icmp_rtime;
      goto icmp_reflect;

    case ICMP_MASKREQ:
      if( ((ngIp_flags & NG_ICMPO_MASKREPLY) == 0)
          || (netp->if_addr == INADDR_ANY)) goto drop;
      icp->icmp_type = ICMP_MASKREPLY;
      icp->icmp_mask = netp->if_subnetmask;
      /* reply to the message */
    icmp_reflect:
      /* swap source and dest address */
      tmpaddr = ip->ip_dst;
      ip->ip_dst = ip->ip_src;
      /* use dst as the src except for broadcast or multicast */
      if( NG_IN_MULTICAST( tmpaddr)) {
        tmpaddr = NG_INADDR_ANY;
      }
      else {
        NGifnet *tnetp;
        tnetp = ngIfList;
        while( tnetp != NULL) {
          if( tmpaddr == tnetp->if_addr) break;
          if( (tnetp->if_flags & NG_IFF_BROADCAST) &&
              (tmpaddr == tnetp->if_broadaddr)) {
            tmpaddr = tnetp->if_addr;
            break;
          }
          tnetp = tnetp->if_next;
        }
      }
      ip->ip_src = tmpaddr;
      /* reset ttl*/
      ip->ip_ttl = ngIp_ttl;
      /* checksum */
      icp->icmp_cksum = 0;
      icp->icmp_cksum = ngIpCksum( (NGubyte *) icp, ip->ip_len - hlen, NULL, 0);
      /* send message */
#ifdef NG_DEBUG
      ngDebug( NG_DBG_ICMP, NG_DBG_OUTPUT, 0, "%I > %I %s (%d)",
        ip->ip_src, ip->ip_dst,
        icp->icmp_type < sizeof(icmp_type_str)/sizeof(icmp_type_str[0]) ?
        icmp_type_str[icp->icmp_type] : "Invalid Type", icp->icmp_code);
#endif
#ifdef NG_IPSTATS_SUPPORTED
      ngIcmpStat.icps_opackets++;
      if( icp->icmp_type < sizeof( ngIcmpStat.icps_outhist)/
                           sizeof( ngIcmpStat.icps_outhist[0]) ) {
        ngIcmpStat.icps_outhist[icp->icmp_type]++;
      }
#endif
      ngIpOutput( bufp, 0);
      return;

      /* routing */
    case ICMP_REDIRECT:
      break;
    }

    /* messages are passed to raw ip processing */
  default:
send_rawip:
    if( ngProtoList[NG_PROTO_RAWIP]) {
      (ngProtoList[NG_PROTO_RAWIP]->pr_input_f)( bufp);
      return;
    }
#ifdef NG_IPSTATS_SUPPORTED
    /* packet not delivered */
    ngIpStat.ips_delivered--;
#ifdef NG_IGMP_SUPPORTED
    if( (ip->ip_p != IPPROTO_ICMP) &&
        (ip->ip_p != IPPROTO_IGMP))
#else
    if( ip->ip_p != IPROTO_ICMP)
#endif
    {
      ngIpStat.ips_noproto++;
    }
    else {
      ngIpStat.ips_idrops++;
    }
#endif
    goto drop;
  }

bad:
#ifdef NG_DEBUG
  ngDebug( NG_DBG_IP, NG_DBG_INPUT, 1, "Bad Packet");
#endif
#ifdef NG_IPSTATS_SUPPORTED
  ngIpStat.ips_ierrors++;
#endif
drop:
  /* free the buffer */
#ifdef NG_IPFRAG_SUPPORTED
  if( bufp->buf_flags & NG_BUFF_MOREDATA) {
    while( bufp) {
      tbufp = bufp->buf_next;
      ngBufFree( bufp);
      bufp = tbufp;
    }
    return;
  }
#endif
  ngBufFree( bufp);
  return;
}

/*****************************************************************************
 * ngProto_IP
 *****************************************************************************
 * IP Protocol Definition Structure
 */

const NGproto ngProto_IP = {
  "IP",
  0,
  0,
  NG_PROTO_IP,
  ipInit,
  ipCntl,
  ipTimer,
  ipInput,
  NULL,
  NULL,
  NULL
};

