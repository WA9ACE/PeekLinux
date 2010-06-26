/*****************************************************************************
 * $Id: ngtcp.h,v 1.10 2002/04/25 08:37:56 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * TCP Module Definitions
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
 * 13/09/99 -
 *  prototype of ngTcpReass() changed
 * 20/03/2000 -
 *  added test for shc compiler
 * 30/05/2000 -
 *  replaced endianness test with NG_XXX_ENDIAN_BITFIELD test
 * 08/06/2000 -
 *  tcb_idle,tcb_rtt,tcb_srtt,tcb_rttvar are signed
 * 19/07/2000 -
 *  added protocol structure
 * 17/11/2000 -
 *  added statistics
 * 23/01/2001 -
 *  removed ti_x2/ti_off bitfield - not ansi compliant
 *  replaced by ti_x2off (see also ngip/tcp.h)
 * 22/02/2001 -
 *  added windows scaling,timestamp,sack options
 *  added TIME WAIT socket lists and options
 * 06/03/2001 -
 *  added NG_TCPO_ROUTE_UPDATE
 * 17/04/2001 -
 *  tcb_idle, tcb_rtt* are (int)
 *****************************************************************************/

#ifndef __NG_NGTCP_H_INCLUDED__
#define __NG_NGTCP_H_INCLUDED__

#include <ngip.h>

/* NG_BEGIN_DECLS // confuses Source Insight */

/* pseudo tcp-ip header */
typedef struct {
    NGuint ti_zero1;
    NGuint ti_zero2;
    NGubyte ti_zero3;
    NGubyte ti_pr;
    NGushort ti_len;
    NGuint ti_src; /* source address */
    NGuint ti_dst; /* destination address */
    NGushort ti_sport; /* source port */
    NGushort ti_dport; /* destination port */
    NGuint ti_seq; /* sequence number */
    NGuint ti_ack; /* acknowledgement number */
    NGubyte ti_x2off; /* data offset */
    NGubyte ti_flags; /* ACK, FIN, PUSH, RST, SYN, URG */
    NGushort ti_win; /* advertised window */
    NGushort ti_sum; /* checksum */
    NGushort ti_urp; /* urgent offset */
} NGtcpiphdr;

/* TCP control block */
typedef struct {
    NGnode tcb_node; /* link */
    NGsock *tcb_sock; /* associated socket structure */
    int tcb_state; /* connection state */
#define NG_TCPS_CLOSED 0
#define NG_TCPS_LISTEN 1
#define NG_TCPS_SYN_SENT 2
#define NG_TCPS_SYN_RECEIVED 3
#define NG_TCPS_ESTABLISHED 4
#define NG_TCPS_CLOSE_WAIT 5
#define NG_TCPS_FIN_WAIT_1 6
#define NG_TCPS_CLOSING 7
#define NG_TCPS_LAST_ACK 8
#define NG_TCPS_FIN_WAIT_2 9
#define NG_TCPS_TIME_WAIT 10
    int tcb_flags; /* internal flags */
#define NG_TF_ACKNOW 0x0001 /* force immediate ACK */
#define NG_TF_DELACK 0x0002 /* delayed ACK timer activated */
#define NG_TF_NODELAY 0x0004 /* disable Nagle algorithm */
#define NG_TF_SENTFIN 0x0008 /* we have sent FIN */
#define NG_TF_USE_SCALE 0x0010 /* request/use window scaling */
#define NG_TF_SEND_TSTMP 0x0020 /* request/send timestamps */
#define NG_TF_SACK_PERMIT 0x0040 /* remote is ok to receive our SACKs */
    /* timers */
    unsigned tcb_tm_rexmt; /* retransmission timer */
    unsigned tcb_tm_persist; /* persist timer */
    unsigned tcb_tm_keep; /* keepalive or connection timer */
    unsigned tcb_tm_2msl; /* 2MSL or FIN_WAIT_2 timer */
    int tcb_rxtshift;
    int tcb_rxtcur;
    int tcb_dupacks;
    int tcb_maxseg; /* maximum segment size */
    int tcb_force;
    /* sequence numbers */
    NGuint tcb_snd_una; /* send unacknowledge */
    NGuint tcb_snd_nxt; /* send next */
    NGuint tcb_snd_up; /* send urgent pointer */
    NGuint tcb_snd_wl1; /* window update seg seq number */
    NGuint tcb_snd_wl2; /* window update seg ack number */
    NGuint tcb_iss; /* initial send sequence number */
    NGuint tcb_snd_wnd; /* send window */
    NGuint tcb_rcv_wnd; /* receive window */
    NGuint tcb_rcv_nxt; /* receive next */
    NGuint tcb_rcv_up; /* receive urgent pointer */
    NGuint tcb_irs; /* initial receive sequence number */
    NGuint tcb_rcv_adv; /* advertised window */
    NGuint tcb_snd_max; /* hightest seq number sent */
    /* congestion control */
    NGuint tcb_snd_cwnd;
    NGuint tcb_snd_ssthresh;
    /* transmit timing */
    int tcb_idle;
    int tcb_rtt;
    int tcb_srtt;
    int tcb_rttvar;
    int tcb_rttmin;
    NGuint tcb_rtseq;
    NGuint tcb_max_sndwnd;
    /* out-of-band data */
    int tcb_oobflags;
#define NG_TCPOOB_HAVEDATA 0x0001
#define NG_TCPOOB_HADDATA 0x0002
    int tcb_iobc;
    int tcb_softerror;
    /* reassembly queue */
    NGnode tcb_reassq;
    /* send buffer management */
    int tcb_buf_ackcc; /* data acknoledged in first buffer */
    NGbuf *tcb_buf_waitack; /* buffers sent but not yet acknoledged */
    /* tcp/ip header template */
    NGtcpiphdr tcb_thdr;
} NGtcpcb;

/*
 * TCP default parameters
 */
/* default buffers size */
#define NG_TCP_BUF_RECV (4*1460)
#define NG_TCP_BUF_SEND (4*1460)
/* send sequence number */
#define NG_TCP_ISS_INIT 1
#define NG_TCP_ISS_INCR 64000
/* segments */
#define NG_TCP_MSS 536 /* max segment size (default = 576 - 40) */
#define NG_TCP_MSL (30*NG_TIMERHZ) /* max segment lifetime (30 sec) */
/* retransmission timer */
#define NG_TCP_REXMT_MIN NG_TIMERHZ /* min value of retransmission timer (1 sec) */
#define NG_TCP_REXMT_MAX (64*NG_TIMERHZ) /* max value of retransmission timer (64 sec) */
#define NG_TCP_REXMT_THRESH 3 /* number of duplicate ACK to trigger fast retransmit */
#define NG_TCP_REXMT_MAXSHIFT 12 /* max nb retransmissions waiting for an ACK */
/* persist timer */
#define NG_TCP_PERS_MIN (5*NG_TIMERHZ) /* min value of persist timer (5 sec) */
#define NG_TCP_PERS_MAX (60*NG_TIMERHZ) /* max value of persist timer (60 sec) */
/* rtt estimator */
#define NG_TCP_SRTT_BASE 0
#define NG_TCP_SRTT_DFLT (3*NG_TIMERHZ) /* default rtt (3 sec) */
#define NG_TCP_RTT_SCALE 8
#define NG_TCP_RTT_SHIFT 3
#define NG_TCP_RTTVAR_SCALE 4
#define NG_TCP_RTTVAR_SHIFT 2
/* keepalive timer */
#define NG_TCP_KEEP_INIT (75*NG_TIMERHZ) /* connection establishment timer value (75 sec) */
#define NG_TCP_KEEP_IDLE (7200*NG_TIMERHZ) /* idle time before first probe (2 hours) */
#define NG_TCP_KEEP_INTVL (75*NG_TIMERHZ) /* interval between probes when no response (75 sec) */
#define NG_TCP_KEEP_MAXCNT 8 /* max keepalive probes when no response */
                                                /* time after probing before giving up: */
#define NG_TCP_KEEP_MAXIDLE (NG_TCP_KEEP_MAXCNT*NG_TCP_KEEP_INTVL)
/* SO_LINGER */
#define NG_TCP_LINGERTIME (120*NG_TIMERHZ) /* max seconds for SO_LINGER option (120 sec) */

/* configuration options */
#define NG_TCPO_TCB_MAX 0x0420
#define NG_TCPO_TCB_TABLE 0x0421
#define NG_TCPO_RCVD_BUF 0x0422
#define NG_TCPO_SEND_BUF 0x0423
#define NG_TCPO_USE_SCALE 0x0424
#define NG_TCPO_SEND_TSTMP 0x0425
#define NG_TCPO_DO_SACK 0x0426
#define NG_TCPO_TIMEWAIT_TABLE 0x0427
#define NG_TCPO_TIMEWAIT_MAX 0x0428
#define NG_TCPO_ROUTE_UPDATE 0x0429

/* TCP options used by setsockopt/getsockopt */
#define NG_TCP_NODELAY 0x0001 /* don't delay send to coalesce packets */
#define NG_TCP_MAXSEG 0x0002 /* set maximum segment size */
#define NG_TCP_USE_SCALE 0x0003
#define NG_TCP_SEND_TSTMP 0x0004
#define NG_TCP_SACK_DISABLE 0x0005

#ifndef NG_NO_BSDSOCK_DECLS

/* BSD compatible names */
#define TCP_NODELAY NG_TCP_NODELAY
#define TCP_MAXSEG NG_TCP_MAXSEG
#define TCP_SACK_DISABLE NG_TCP_SACK_DISABLE

#endif /* NG_NO_BSDSOCK_DECLS */

/* protocol structure */
extern const NGproto ngProto_TCP;

/*****************************************************************************
 * Private Section
 *****************************************************************************/

/* internal prototypes */
int ngTcpOutput( NGtcpcb *tp);
NGtcpcb *ngTcpClose( NGtcpcb *tp);
NGtcpcb *ngTcpDrop( NGtcpcb *tp, int err);
void ngTcpSetPersist( NGtcpcb *tp);
void ngTcpXmitTimer( NGtcpcb *tp, int rtt);
int ngTcpReass( NGtcpcb *tp, NGuint bufseq, int tcpflags, NGbuf *bufp);

/* statistics */
typedef struct {
  unsigned long tcps_connattempts; /* # of connections initiated */
  unsigned long tcps_accepts; /* # of connections accepted */
  unsigned long tcps_conndrops; /* # of embryonic connections dropped */
  unsigned long tcps_drops; /* # of established connections dropped */
  unsigned long tcps_ipackets; /* # of segments received */
  unsigned long tcps_ierrors; /* # of segments received with errors */
  unsigned long tcps_opackets; /* # of segments sent */
  unsigned long tcps_orexmts; /* # of segments retransmitted */
  unsigned long tcps_oresets; /* # of segments sent with RST flag */
  unsigned long tcps_idupacks; /* # of duplicate ack recvd */
  unsigned long tcps_ioopackets; /* # of segments recvd out-of-order */
} NGtcpstat;

/* global variables */
extern int ngTcpBufDataOffset; /* offset of tcp data in message buffers */
extern NGuint ngTcp_Iss; /* initial send sequence number */
extern const int ngTcp_Backoff[]; /* retransmit timer table */
extern NGnode ngTcp_Sockq; /* sockets in use */
extern NGqueue ngTcp_TcpcbFreeq; /* tcp control blocks (free) */
extern NGsock *ngTcp_LastSock; /* last socket used */
#ifdef NG_IPSTATS_SUPPORTED
extern NGtcpstat ngTcpStat; /* statistics */
#endif

/* sequence number compare macros */
#define NG_SEQ_LT( a, b) ((NGint)((a)-(b)) < 0)
#define NG_SEQ_LE( a, b) ((NGint)((a)-(b)) <= 0)
#define NG_SEQ_GT( a, b) ((NGint)((a)-(b)) > 0)
#define NG_SEQ_GE( a, b) ((NGint)((a)-(b)) >= 0)

/* timer macros */
#define NG_TCPT_RANGESET( tv, value, tvmin, tvmax) { \
    (tv) = (value); \
    if( (tv) < (tvmin)) \
        (tv) = (tvmin); \
    else if( (tv) > (tvmax)) \
        (tv) = (tvmax); \
}
#define NG_TCP_REXMTVAL( tp) \
  (((tp)->tcb_srtt >> NG_TCP_RTT_SHIFT) + (tp)->tcb_rttvar)

/* free all waiting send buffers */
#define NG_TCPBUF_FREE( tp) { \
    NGbuf *bufp; \
    int ictl; \
    bufp = (tp)->tcb_buf_waitack; \
    while( bufp) { \
        (tp)->tcb_buf_waitack = bufp->buf_next; \
        ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE); \
        if( bufp->buf_flags & NG_BUFF_BUSY) { \
            bufp->buf_flags &= ~(NG_BUFF_BUSY|NG_BUFF_DONTFREE); \
            ngOSIntrCtl( ictl); \
        } \
        else { \
            ngOSIntrCtl( ictl); \
            ngBufFree( bufp); \
        } \
        bufp = (tp)->tcb_buf_waitack; \
    } \
}

/* NG_END_DECLS // confuses Source Insight */

#endif

