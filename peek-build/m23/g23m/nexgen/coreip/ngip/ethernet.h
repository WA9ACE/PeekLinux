/*****************************************************************************
 * $Id: ethernet.h,v 1.3 2001/11/20 14:36:07 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Ethernet Protocol
 * Low Level Definitions
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
 * 20/11/2001 -
 *  added constants for Ethernet header elements size
 *****************************************************************************/

#ifndef __NG_ETHERNET_H_INCLUDED__
#define __NG_ETHERNET_H_INCLUDED__

/* NG_BEGIN_DECLS // confuses Source Insight */

#define ETHER_ADDR_LEN 6 /* length of Ethernet address */
#define ETHER_TYPE_LEN 2 /* length of Ethernet type field */
#define ETHER_HDR_LEN ((2*ETHER_ADDR_LEN)+ETHER_TYPE_LEN)
#define ETHER_CRC_LEN 4 /* length of Ethernet CRC */
#define ETHER_MIN_LEN 64 /* minimum frame length */
#define ETHER_MAX_LEN 1518 /* maximum frame length */

#define ETHERMTU (ETHER_MAX_LEN-ETHER_HDR_LEN-ETHER_CRC_LEN)
#define ETHERMIN (ETHER_MIN_LEN-ETHER_HDR_LEN-ETHER_CRC_LEN)

/* ethernet header */
typedef struct {
    NGubyte ether_dhost[ETHER_ADDR_LEN]; /* destination address */
    NGubyte ether_shost[ETHER_ADDR_LEN]; /* source address */
    NGushort ether_type; /* frame type */
#define ETHERTYPE_IP 0x0800 /* IP protocol */
#define ETHERTYPE_ARP 0x0806 /* Address Resolution Protocol */
#define ETHERTYPE_REVARP 0x8035 /* Reverse Addr Resolution Protocol */
#define ETHERTYPE_VLAN 0x8100 /* 802.1Q VLAN tagging */
#define ETHERTYPE_PPPOEDISC 0x8863 /* PPP Over Ethernet Discovery Stage */
#define ETHERTYPE_PPPOE 0x8864 /* PPP Over Ethernet Session Stage */
} NGetherhdr;

/* NG_END_DECLS // confuses Source Insight */

#endif

