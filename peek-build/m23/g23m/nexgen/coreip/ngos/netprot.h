/*****************************************************************************
 * $Id: netprot.h,v 1.4 2002/06/25 13:39:53 dg Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Generic Protocols Definition
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
 * 27/11/98 - Regis Feneon
 * 29/12/98 -
 *  Added link information and protocol control function
 * 07/01/99 -
 *  User options function added
 * 19/07/2000 -
 *  NG_PRU_XXX and NG_PRC_XXX moved here
 *  added PPPoE protocol number
 * 24/11/2000 -
 *  pr_next field removed
 * 27/03/2001 -
 *  support for resolver
 * 22/05/2002 - dg
 *  support for ipsec
 *****************************************************************************/

#ifndef __NG_NETPROT_H_INCLUDED__
#define __NG_NETPROT_H_INCLUDED__

#include <ngos/netbuf.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* Generic protocol structure */
typedef struct NGproto_S {
    const char *pr_name; /* protocol ascii name */
    int pr_type; /* type (see net_sock.h) */
    int pr_flags; /* protocol flags */
#define NG_PR_ATOMIC 0x0001
#define NG_PR_ADDR 0x0002
#define NG_PR_CONNREQUIRED 0x0004
#define NG_PR_WANTRCVD 0x0008
    int pr_protocol; /* internal protocol number */
#define NG_PROTO_UNKNOWN 0
#define NG_PROTO_IP 1
#define NG_PROTO_RAWIP 2
#define NG_PROTO_UDP 3
#define NG_PROTO_TCP 4
#define NG_PROTO_ARP 5
#define NG_PROTO_RARP 6
#define NG_PROTO_PPP 7
#define NG_PROTO_DHCP 8
#define NG_PROTO_DHCPS 9
#define NG_PROTO_PPPOE 10
#define NG_PROTO_RESOLV 11
#define NG_PROTO_IPSEC 12
#define NG_PROTO_MAX 16
    void (*pr_init_f)( void); /* initialization function */
    int (*pr_cntl_f)( int cmd, int option, void *arg);
    void (*pr_timer_f)( void); /* 200ms timer function */
    void (*pr_input_f)( NGbuf *); /* input function */
    int (*pr_useropt_f)(); /* user options processing */
    void (*pr_error_f)(); /* network errors processing function */
    int (*pr_user_f)(); /* user interface function */
} NGproto;

/* command codes for pr_user_f() */
#define NG_PRU_ABORT 1
#define NG_PRU_ATTACH 2
#define NG_PRU_BIND 3
#define NG_PRU_CONNECT 4
#define NG_PRU_DETACH 5
#define NG_PRU_DISCONNECT 6
#define NG_PRU_LISTEN 7
#define NG_PRU_RCVD 8
#define NG_PRU_RCVOOB 9
#define NG_PRU_SEND 10
#define NG_PRU_SENDBUF 11
#define NG_PRU_SENDBUFINI 12
#define NG_PRU_SENDOOB 13
#define NG_PRU_SHUTDOWN 14

/* error codes for pr_ctrl_f() */
#define NG_PRC_HOSTDEAD 1
#define NG_PRC_IFDOWN 2
#define NG_PRC_MSGSIZE 3
#define NG_PRC_PARAMPROB 4
#define NG_PRC_QUENCH 5
#define NG_PRC_REDIRECT_HOST 6
#define NG_PRC_REDIRECT_NET 7
#define NG_PRC_ROUTEDEAD 8
#define NG_PRC_TIMXCEED 9
#define NG_PRC_UNREACH_NET 10
#define NG_PRC_UNREACH_HOST 11
#define NG_PRC_UNREACH_PORT 12
#define NG_PRC_UNREACH_PROTOCOL 13
#define NG_PRC_UNREACH_SRCFAIL 14

/* NG_END_DECLS // confuses Source Insight */

#endif

