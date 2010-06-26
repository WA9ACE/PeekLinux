/*****************************************************************************
 * $Id: ngip.h,v 1.7.2.1 2002/10/28 18:49:49 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet Protocol Module Definitions
 *----------------------------------------------------------------------------
 *      Copyright (c) 1998-2002 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *      This software is  supplied  under  the terms of a
 *      license agreement or nondisclosure agreement with
 *      NexGen Software, and may not be copied or disclosed
 *      except  in  accordance  with  the  terms of  that
 *      agreement.
 *
 *----------------------------------------------------------------------------
 * 07/10/98 - Regis Feneon
 * 29/12/98 -
 *  added configuration options
 *  changes in global variable names
 * 13/01/99 -
 *  standart ip address class definitions added (moved from ip.h)
 *  net_in.h removed
 * 09/02/2000 -
 *  added NG_IN_ADDRXXX() macros in network-byte order
 * 18/02/2000 - Adrien Felon
 *  ngIpOuput() has new argument "flags" for IP forwarding and NAT support
 * 14/04/2000 -
 *  defining IP options for NAT configuration
 * 19/07/2000 - Regis Feneon
 *  added inet and checksum routines prototypes
 *  added host/network byte order macros
 *  added ip and rawip protocols structure
 * 18/08/2000 -
 *  added NG_INADDR() macro
 * 13/10/2000 - Adrien Felon
 *  removed NG_IPO_NAT_ADDEXTIF
 *  added NG_IPO_NAT_IFPUB NG_IPO_NAT_IFPUB
 * 02/11/2000 - Regis Feneon
 *  added IGMP definitions
 * 16/11/2000 -
 *  added IP statistics
 * 24/11/2000 -
 *  NGproto,ngProtoList changed, ngIpProtoTable removed
 *  added ngIpId
 * 06/03/2001 -
 *  added NG_IPO_NETDOWN
 * 25/05/2001 -
 *  added ngIpTmpIov[]
 * 20/11/2001 -
 *  in_cksum() renamed ngIpCksum()
 * 28/10/2002 -
 *  added prototypes for ngProtoSet/GetOption()
 *****************************************************************************/

#ifndef __NG_NGIP_H_INCLUDED__
#define __NG_NGIP_H_INCLUDED__

#include <ngos.h>
#include <ngnet.h>
#include <ngmemblk.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* NexGenIP library generation options */
/* Warning: the libraries need to be rebuilt when options are changed */
#ifndef NGIP_SMALL_SIZE_LIB
#define NG_IPFRAG_SUPPORTED
#define NG_IPFWD_SUPPORTED
#define NG_IGMP_SUPPORTED
#define NG_IPSTATS_SUPPORTED
#endif

/*
 * Public
 */

#ifndef NG_NO_BSDSOCK_DECLS

/* Definitions of bits in internet address */
/* (!) addresses are in host-byte order (!) */
/* this should be in ngsocket .h */

#define IN_CLASSA(i) (((NGuint)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET 0xff000000
#define IN_CLASSA_NSHIFT 24
#define IN_CLASSA_HOST 0x00ffffff
#define IN_CLASSA_MAX 128

#define IN_CLASSB(i) (((NGuint)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET 0xffff0000
#define IN_CLASSB_NSHIFT 16
#define IN_CLASSB_HOST 0x0000ffff
#define IN_CLASSB_MAX 65536

#define IN_CLASSC(i) (((NGuint)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET 0xffffff00
#define IN_CLASSC_NSHIFT 8
#define IN_CLASSC_HOST 0x000000ff

#define IN_CLASSD(i) (((NGuint)(i) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(i) IN_CLASSD(i)
#define IN_MULTICAST_H(i) (((NGuint)(i) & 0xf0) == 0xe0)

#define IN_EXPERIMENTAL(i) (((NGuint)(i) & 0xe0000000) == 0xe0000000)
#define IN_BADCLASS(i) (((NGuint)(i) & 0xf0000000) == 0xf0000000)

#define INADDR_ANY (NGuint)0x00000000
#define INADDR_LOOPBACK 0x7f000001
#define INADDR_BROADCAST (NGuint)0xffffffff
#define INADDR_NONE 0xffffffff

#define INADDR_UNSPEC_GROUP 0xe0000000
#define INADDR_ALLHOSTS_GROUP 0xe0000001
#define INADDR_MAX_LOCAL_GROUP 0xe00000ff

#endif /* NG_NO_BSDSOCK_DECLS */

/* the same in network-byte order */
#if defined (NG_BIG_ENDIAN)
#define NG_IN_CLASSA(a) (((NGuint)(a) & 0x80000000) == 0)
#define NG_IN_CLASSA_NET 0xff000000
#define NG_IN_CLASSB(a) (((NGuint)(a) & 0xc0000000) == 0x80000000)
#define NG_IN_CLASSB_NET 0xffff0000
#define NG_IN_CLASSC(a) (((NGuint)(a) & 0xe0000000) == 0xc0000000)
#define NG_IN_CLASSC_NET 0xffffff00
#define NG_IN_CLASSD(a) (((NGuint)(a) & 0xf0000000) == 0xe0000000)
#define NG_IN_BADCLASS(a) (((NGuint)(a) & 0xf0000000) == 0xf0000000)
#define NG_INADDR_LOOPBACK 0x7f000001
#define NG_INADDR(a,b,c,d) \
    ((((NGuint)(a))<<24)|(((NGuint)(b))<<16)|(((NGuint)(c))<<8)|((NGuint)(d)))
#elif defined (NG_LITTLE_ENDIAN)
#define NG_IN_CLASSA(a) (((NGuint)(a) & 0x00000080UL) == 0x00000000UL)
#define NG_IN_CLASSA_NET (0x000000ffUL)
#define NG_IN_CLASSB(a) (((NGuint)(a) & 0x000000c0UL) == 0x00000080UL)
#define NG_IN_CLASSB_NET (0x0000ffffUL)
#define NG_IN_CLASSC(a) (((NGuint)(a) & 0x000000e0UL) == 0x000000c0UL)
#define NG_IN_CLASSC_NET (0x00ffffffUL)
#define NG_IN_CLASSD(a) (((NGuint)(a) & 0x000000f0UL) == 0x000000e0UL)
#define NG_IN_BADCLASS(a) (((NGuint)(a) & 0x000000f0UL) == 0x000000f0UL)
#define NG_INADDR_LOOPBACK (0x0100007fUL)
#define NG_INADDR(a,b,c,d) \
    ((((NGuint)(d))<<24)|(((NGuint)(c))<<16)|(((NGuint)(b))<<8)|((NGuint)(a)))
#else
#error "NG_XXX_ENDIAN macro not defined"
#endif
#define NG_INADDR_ANY (NGuint)0x00000000
#define NG_INADDR_BROADCAST (NGuint)0xffffffff
#define NG_IN_MULTICAST(a) NG_IN_CLASSD(a)

/* multicast groups */
#if defined (NG_BIG_ENDIAN)
#define NG_INADDR_UNSPEC_GROUP 0xe0000000
#define NG_INADDR_ALLHOSTS_GROUP 0xe0000001
#define NG_INADDR_MAX_LOCAL_GROUP 0xe00000ff
#else
#define NG_INADDR_UNSPEC_GROUP 0x000000e0
#define NG_INADDR_ALLHOSTS_GROUP 0x010000e0
#define NG_INADDR_MAX_LOCAL_GROUP 0xff0000e0
#endif

/* protocols */
#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_IGMP 2
#define IPPROTO_GGP 3
#define IPPROTO_TCP 6
#define IPPROTO_EGP 8
#define IPPROTO_PUP 12
#define IPPROTO_UDP 17
#define IPPROTO_IDP 22
#define IPPROTO_TP 29
#define IPPROTO_EON 80
#define IPPROTO_RAW 255
#define IPPROTO_MAX 256

/* udp and tcp reserved ports */
#define IPPORT_RESERVED 1024
#define IPPORT_USERRESERVED 5000

#define IPPORT_ECHO 7
#define IPPORT_DISCARD 9
#define IPPORT_SYSTAT 11
#define IPPORT_DAYTIME 13
#define IPPORT_NETSTAT 15
#define IPPORT_FTP 21
#define IPPORT_TELNET 23
#define IPPORT_SMTP 25
#define IPPORT_TIMESERVER 37
#define IPPORT_NAMESERVER 42
#define IPPORT_WHOIS 43
#define IPPORT_MTP 57
#define IPPORT_BOOTP 67
#define IPPORT_BOOTPC 68
#define IPPORT_TFTP 69
#define IPPORT_RJE 77
#define IPPORT_FINGER 79
#define IPPORT_TTYLINK 87
#define IPPORT_SUPDUP 95

/* route entry */
typedef struct {
    NGifnet *rt_ifnetp;
    NGuint rt_gateway;
    NGuint rt_addr;
    NGuint rt_subnetmask;
} NGiprtent;

/* IP options */
/* Warning: values from 440 to 449 are used by NAT module (see below) */
#define NG_IPO_TTL 0x0400
#define NG_IPO_TOS 0x0401
#define NG_IPO_FRAG_TIMEO 0x0403
#define NG_IPO_ROUTE_DEFAULT 0x0404
#define NG_IPO_ROUTE_MAX 0x0405
#define NG_IPO_ROUTE_TABLE 0x0406
#define NG_IPO_ROUTE_ADD 0x0407
#define NG_IPO_ROUTE_DELETE 0x0408
#define NG_IPO_NETDOWN 0x040c
#define NG_IPO_FORWARD 0x0481
#define NG_IPO_SENDREDIRECT 0x0482
#define NG_ICMPO_MASKREPLY 0x0484
#define NG_UDPO_CHECKSUM 0x0488
#define NG_IPO_FLAGSMASK 0x007f

/* Following are used only if NAT forwarding is supported */
#define NG_IPO_NAT_TABLEMAX 0x0440
#define NG_IPO_NAT_TABLE 0x0441
#define NG_IPO_NAT_ALGMAX 0x0442
#define NG_IPO_NAT_ALG 0x0443
#define NG_IPO_NAT_IFPUB 0x0444
#define NG_IPO_NAT_IFPRIV 0x0445
#define NG_IPO_NAT_TO_TCP 0x0446
#define NG_IPO_NAT_TO_TCP_FIN 0x0447
#define NG_IPO_NAT_TO_UDP 0x0448
#define NG_IPO_NAT_TO_ICMP 0x0449

/* Multicast group entry */
typedef struct NGinmulti_S {
    struct NGinmulti_S *inm_next; /* pointer to next group */
    NGifnet *inm_ifnetp; /* associated interface */
    NGuint inm_addr; /* address */
    int inm_refcount; /* nb of references to this address */
    int inm_timer; /* report timer */
} NGinmulti;

/* IGMP/Multicast options */
#define NG_IPO_INMULTI_MAX 0x0409
#define NG_IPO_INMULTI_TABLE 0x040a

/* Prototypes */
int ngRouteAdd( NGuint dest, NGuint subnet, NGuint gateway);
int ngRouteDelete( NGuint dest);
int ngRouteDefault( NGuint gateway);

int ngInetATON( const char *str, NGuint *addr);
int ngInetNTOA( NGuint addr, char *buffer, int buflen);
NGushort ngInetChecksum( void *ptr, int len);

/* host/network byte order conversions */
#define ngHTONS( val) ngHTOBE16( val)
#define ngHTONL( val) ngHTOBE32( val)
#define ngNTOHS( val) ngBETOH16( val)
#define ngNTOHL( val) ngBETOH32( val)
#define ngConstHTONS( val) ngConstHTOBE16( val)
#define ngConstHTONL( val) ngConstHTOBE32( val)
#define ngconstNTOHS( val) ngConstBETOH16( val)
#define ngConstNTOHL( val) ngConstBETOH32( val)

/* IP protocol(s) */
extern const NGproto ngProto_IP;
extern const NGproto ngProto_RAWIP;

/* set/get protocol options */
int ngProtoGetOption( int proto, int option, void *optval);
int ngProtoSetOption( int proto, int option, const void *optval);

/*
 * Private
 */

/* incoming fragments reassembly structure */
typedef struct {
    NGnode ipf_node; /* link to fragments */
    int ipf_ttl; /* time to live */
    int ipf_p; /* ip protocol */
    u_int ipf_id; /* fragment id */
    NGuint ipf_src; /* source address */
    NGuint ipf_dst; /* destination address */
    int ipf_hlen; /* first fragment ip header length */
} NGipfrag;

#ifdef NG_IPFRAG_SUPPORTED
#define NG_IPFRAG_NBMAX 4
#define NG_IPFRAG_SIZEMAX (8*1500)
#else
#define NG_IPFRAG_NBMAX 1
#define NG_IPFRAG_SIZEMAX ngBufDataMax
#endif

/* statistics */
typedef struct {
  unsigned long ips_ipackets; /* # of input datagrams */
  unsigned long ips_ierrors; /* # of input datagrams with errors */
  unsigned long ips_idrops; /* # of dgrams discarded because lack of resources */
  unsigned long ips_cantforward; /* # of dgrams with invalid dest address */
  unsigned long ips_forward; /* # of dgrams forwarded */
  unsigned long ips_noproto; /* # of dgrams to an unsupported protocol */
  unsigned long ips_delivered; /* # of dgrams successfully delivered */
  unsigned long ips_opackets; /* # of dgrams sent (not forwarded) */
  unsigned long ips_odrops; /* # of output dgrams discarded because no resource */
  unsigned long ips_noroute; /* # of output dgrams with no dest route */
  unsigned long ips_ifragments; /* # of fragments received */
  unsigned long ips_reassembled; /* # of datagrams reassembled */
  unsigned long ips_reassfailed; /* # of reassembly failures */
  unsigned long ips_fragmented; /* # of emitted dgrams fragmented */
  unsigned long ips_cantfrag; /* # of dgrams discarded because of fragmentation */
  unsigned long ips_ofragments; /* # of emitted fragments */
} NGipstat;

typedef struct {
  unsigned long icps_ipackets; /* # icmp received */
  unsigned long icps_ierrors; /* # received with bad header */
  unsigned long icps_inhist[20]; /* # received by types */
  unsigned long icps_opackets; /* # icmp sent */
  unsigned long icps_oerrors; /* # icmp cant be sent */
  unsigned long icps_outhist[20]; /* # sent by types */
} NGicmpstat;

typedef struct {
  unsigned long igps_ipackets; /* # igmp msg received */
  unsigned long igps_ierrors; /* # rcvd with bad header */
  unsigned long igps_iourreports; /* # rcvd as report for local groups */
  unsigned long igps_iqueries; /* # queries rcvd */
  unsigned long igps_ireports; /* # reports rcvd */
  unsigned long igps_oreports; /* # reports sent */
} NGigmpstat;

/* global variables */
extern int ngIp_ttl; /* default ttl */
extern int ngIp_tos; /* default tos */
extern int ngIp_flags; /* global flags */
extern u_int ngIp_frag_timeo; /* reassembly fragment timeout */
extern int ngIp_route_max; /* size of routing table */
extern NGiprtent *ngIp_route_table; /* routing table */
extern NGiprtent ngIp_route_default; /* default gateway */
extern int ngIp_multi_timers; /* IGMP timers are pending */
extern NGmemblk ngIp_multi_free; /* free NGinmulti structures */
extern NGuint ngIp_random;
extern NGushort ngIpId;
#ifdef NG_IPSTATS_SUPPORTED
extern NGipstat ngIpStat; /* statistics */
extern NGicmpstat ngIcmpStat;
#ifdef NG_IGMP_SUPPORTED
extern NGigmpstat ngIgmpStat;
#endif
#endif
extern NGiovec ngIpTmpIov[];

extern NGnode ngRawip_Sockq; /* raw sockets in use */

/* IP function prototypes */
int ngIpOutput( NGbuf *bufp, int flags);
void ngIpLoopback( NGifnet *netp, NGbuf *bufp);

/* Defining ngIpOutput() flags */
#define NG_IPOUTPUT_IPFWD 0x0001 /* only if IPFWD supported */
#define NG_IPOUTPUT_NATFWD 0x0002 /* only if both IPFWD and NATFWD supported */
#define NG_IPOUTPUT_MCASTLOOP 0x0004 /* enable loopback of mcasts */
#define NG_IPOUTPUT_DONTROUTE 0x0008 /* bypass routing table */

/* routing internal functions */
NGifnet *ngIpRouteLookup( NGuint addr, NGuint *gateway, int dontroute);
void ngIpRouteUpdate( void);

/* checksum computation */
NGushort ngIpCksum( void *ptr, int len, NGiovec *iov, int iovcnt);
void ngCksumAdjust( NGubyte *sum, NGubyte *optr, int olen,
                        NGubyte *nptr, int nlen);

#ifdef NG_IGMP_SUPPORTED
NGinmulti *ngIgmpJoinGroup( NGifnet *netp, NGuint addr);
void ngIgmpLeaveGroup( NGinmulti *inmp);
void ngIgmpSendReport( NGinmulti *inmp);
#endif

/* NG_END_DECLS // confuses Source Insight */

#endif

