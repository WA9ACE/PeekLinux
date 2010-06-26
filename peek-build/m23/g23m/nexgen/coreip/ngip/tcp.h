/*****************************************************************************
 * $Id: tcp.h,v 1.4 2001/04/02 11:12:34 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * TCP Protocol Layer Definitions
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
 * 20/03/2000 -
 *  added test for shc compiler
 * 30/05/2000 -
 *  replaced endianness test with NG_XXX_ENDIAN_BITFIELD test
 * 23/01/2001 -
 *  removed th_x2/th_off bitfield - not ansi compliant
 *  replaced by th_x2off, NG_TCPOFF_MASK, NG_TCPOFF_SHIFT
 *****************************************************************************/

#ifndef __NG_TCP_H_INCLUDED__
#define __NG_TCP_H_INCLUDED__

/* NG_BEGIN_DECLS // confuses Source Insight */

typedef struct {
    NGushort th_sport; /* source port */
    NGushort th_dport; /* destination port */
    NGuint th_seq; /* sequence number */
    NGuint th_ack; /* acknowledgement number */
    NGubyte th_x2off; /* data offset */
#define NG_TCPOFF_MASK 0xf0
#define NG_TCPOFF_SHIFT 4
    NGubyte th_flags; /* ACK, FIN, PUSH, RST, SYN, URG */
#define TH_FIN 0x01
#define TH_SYN 0x02
#define TH_RST 0x04
#define TH_PUSH 0x08
#define TH_ACK 0x10
#define TH_URG 0x20
    NGushort th_win; /* advertised window */
    NGushort th_sum; /* checksum */
    NGushort th_urp; /* urgent offset */
} NGtcphdr;

/* tcp options */
#define TCPOPT_EOL 0
#define TCPOPT_NOP 1
#define TCPOPT_MAXSEG 2
#define TCPOPT_WINDOW 3
#define TCPOPT_SACK_PERMITTED 4
#define TCPOPT_SACK 5
#define TCPOPT_TIMESTAMP 8

#define TCPOPTLEN_MAXSEG 4
#define TCPOPTLEN_WINDOW 3
#define TCPOPTLEN_SACK_PERMITTED 2
#define TCPOPTLEN_SACK 8
#define TCPOPTLEN_TIMESTAMP 10

#define MAX_TCPOPTLEN 40 /* 3 SACKs + TIMESTAMP */

#define TCP_MSS 536 /* default segment size */
#define TCP_MAXWIN 65535UL /* largest value for window */
#define TCP_MAX_WINSHIFT 14 /* maximum window scale fatctor */

/* tcp connection state values */
#define TCPS_CLOSED 0
#define TCPS_LISTEN 1
#define TCPS_SYN_SENT 2
#define TCPS_SYN_RECEIVED 3
#define TCPS_ESTABLISHED 4
#define TCPS_CLOSE_WAIT 5
#define TCPS_FIN_WAIT_1 6
#define TCPS_CLOSING 7
#define TCPS_LAST_ACK 8
#define TCPS_FIN_WAIT_2 9
#define TCPS_TIME_WAIT 10

/* NG_END_DECLS // confuses Source Insight */

#endif

