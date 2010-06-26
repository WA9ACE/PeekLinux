/*****************************************************************************
 * $Id: ngudp.h,v 1.2 2001/04/02 11:08:22 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * UDP Module Definitions
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
 * 10/12/98 - Regis Feneon
 *  Definitions moved from udp.c
 * 25/05/99 -
 *  Added Direct interface routines/macros
 * 19/07/2000 -
 *  added protocol structure
 * 16/11/2000 -
 *  added UDP statistics
 *****************************************************************************/

#ifndef __NG_NGUDP_H_INCLUDED__
#define __NG_NGUDP_H_INCLUDED__

#include <ngip.h>
#include <ngdsock.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* protocol structure */
extern const NGproto ngProto_UDP;

/* default parameters */
#define NG_UDP_INPQMAX 4
#define NG_UDP_TTL 100
#define NG_UDP_TOS 0

/* pseudo udp-ip header */
typedef struct {
    NGuint ui_zero1;
    NGuint ui_zero2;
    NGubyte ui_zero3;
    NGubyte ui_pr;
    NGushort ui_len;
    NGuint ui_src;
    NGuint ui_dst;
    NGushort ui_sport;
    NGushort ui_dport;
    NGushort ui_ulen;
    NGushort ui_sum;
} NGudpiphdr;

/* statistics */
typedef struct {
  unsigned long udps_ipackets; /* # packets received and delivered */
  unsigned long udps_noport; /* # packets w no dest port */
  unsigned long udps_ierrors; /* # bad packets */
  unsigned long udps_opackets; /* # packets sent */
} NGudpstat;

/* global variables */
extern NGnode ngUdp_Sockq;
extern NGushort ngUdp_DirectPort;
extern void (*ngUdp_DirectInput_f)( NGbuf *bufp, NGsockaddr *from);
#ifdef NG_IPSTATS_SUPPORTED
extern NGudpstat ngUdpStat; /* statictics */
#endif

/* Direct interface */
#define ngUdpDirectOpen( port, func) { \
    ngUdp_DirectPort = (port); \
    ngUdp_DirectInput_f = (func); \
}

#define ngUdpDirectBufInit( bufp) \
    ( (int (*)( NGsock *, int, void *, int, NGsockaddr *)) \
    (ngProto_UDP.pr_user_f))( NULL, NG_PRU_SENDBUFINI, (bufp), 0, NULL)

#define ngUdpDirectBufSend( bufp, addr) \
    ( (int (*)( NGsock *, int, void *, int, NGsockaddr *)) \
    (ngProto_UDP.pr_user_f))( NULL, NG_PRU_SENDBUF, (bufp), 0, (addr))

/* NG_END_DECLS // confuses Source Insight */

#endif

