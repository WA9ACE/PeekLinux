/*****************************************************************************
 * $Id: dns.h,v 1.3 2001/04/26 16:45:33 af Exp $
 * $Name: rel_1_3_b_7 $
 *----------------------------------------------------------------------------
 * NexGenRESOLV v1.3
 * Domain Name System Definitions
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
 *
 *    All rights reserved. NexGen Software' source code is an
 *  unpublished work and the use of a copyright notice does not imply
 *  otherwise.  This source code contains confidential, trade secret
 *  material of NexGen Software. Any attempt or participation in
 *  deciphering, decoding, reverse engineering or in any way altering
 *  the source code is strictly prohibited, unless the prior written
 *  consent of NexGen Software is obtained.
 *
 *    This software is supplied under the terms of a license agreement
 *  or nondisclosure agreement with NexGen Software, and may not be
 *  copied or disclosed except in accordance with the terms of that
 *  agreement.
 *
 *----------------------------------------------------------------------------
 * 16/03/2001 - Adrien Felon
 *****************************************************************************/

#ifndef __NG_RESOLV_DNS_H_INCLUDED__
#define __NG_RESOLV_DNS_H_INCLUDED__

#include <ngos.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

#define DNS_ARPALEN_MAX sizeof( "255.255.255.255.in-addr.arpa")

/* DNS query header */
typedef struct {
  NGushort dns_id; /* query identifiant value */
  NGubyte dns_bitf1;
#define DNS_MASK_QR (0x1<<7)
#define DNS_MASK_OPCODE (0x7<<3)
#define DNS_MASK_AA (0x1<<2)
#define DNS_MASK_TC (0x1<<1)
#define DNS_MASK_RD (0x1)
  NGubyte dns_bitf2;
#define DNS_MASK_RA (0x1<<7)
#define DNS_MASK_RCODE (0xf)
  NGushort dns_qdcount; /* number of question entries */
  NGushort dns_ancount; /* number of answer entries */
  NGushort dns_nscount; /* number of authority entries */
  NGushort dns_arcount; /* number of resource entries */
} NGdnshdr;

/* opcodes */
#define DNS_QUERY (0x0<<3) /* standard query */
#define DNS_IQUERY (0x1<<3) /* inverse query */
#define DNS_STATUS (0x2<<3) /* nameserver status query */

/* response codes */
#define DNS_NOERROR 0 /* no error */
#define DNS_FORMERR 1 /* format error */
#define DNS_SERVFAIL 2 /* server failure */
#define DNS_NXDOMAIN 3 /* non existent domain */
#define DNS_NOTIMP 4 /* not implemented */
#define DNS_REFUSED 5 /* query refused */

/* type values */
#define DNS_T_A 1 /* host address */
#define DNS_T_NS 2 /* authoritative server */
#define DNS_T_CNAME 5 /* canonical name */
#define DNS_T_PTR 12 /* domain name pointer */

/* class values */
#define DNS_C_IN 1 /* Internet */
#define DNS_C_ANY 255 /* wildcard */

/* NG_END_DECLS // confuses Source Insight */

#endif /* __NG_RESOLV_DNS_H_INCLUDED__ */

