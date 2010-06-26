/*****************************************************************************
 * $Id: ip.h,v 1.3 2001/04/02 11:12:34 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet Protocol Definitions
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
 * 07/09/98 - Regis Feneon
 * 13/01/99 -
 *  ip address definitions and udp/tcp port list moved to net_ip.h
 * 20/03/2000 -
 *  added test for shc compiler
 * 30/05/2000 -
 *  replaced endianness test with NG_XXX_ENDIAN_BITFIELD test
 * 19/07/2000 -
 *  list of protocols moved to ngip.h
 * 23/01/2001 -
 *  removed (NGubyte) ip_hl/ip_v bit-field - not ansi compliant
 *  replaced by ip_hlv & NG_IPHDRLEN_MASK, NG_IPVER_MASK, NG_IPVER_IPV4
 *****************************************************************************/

#ifndef __NG_IP_H_INCLUDED__
#define __NG_IP_H_INCLUDED__

/* NG_BEGIN_DECLS // confuses Source Insight */

/* internet header */
typedef struct {
    NGubyte ip_hlv; /* header length & version field */
#define NG_IPHDRLEN_MASK 0x0f /* mask for header length */
#define NG_IPVER_MASK 0xf0 /* mask for ip version */
#define NG_IPVER_IPV4 0x40 /* IPv4 version */
#define IPVERSION 4 /* current IP version */
    NGubyte ip_tos; /* type of service */
#define IPTOS_LOWDELAY 0x10 /* minimize delay */
#define IPTOS_THROUGHPUT 0x08 /* maximize throughput */
#define IPTOS_RELIABILITY 0x04 /* maximize reliability */
    NGushort ip_len; /* total length */
    NGushort ip_id; /* identification */
    NGushort ip_off; /* fragment offset field */
#define IPOFF_DF 0x4000 /* dont fragment flag */
#define IPOFF_MF 0x2000 /* more fragment flag */
#define IPOFF_MASK 0x1fff /* mask for fragmenting bits */
    NGubyte ip_ttl; /* time to live */
    NGubyte ip_p; /* protocol */
    NGushort ip_sum; /* checksum */
    NGuint ip_src; /* source address */
    NGuint ip_dst; /* destination address */
} NGiphdr;

#define MAXTTL 255

/* NG_END_DECLS // confuses Source Insight */

#endif

