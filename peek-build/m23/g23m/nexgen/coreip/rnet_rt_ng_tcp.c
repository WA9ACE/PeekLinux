/*****************************************************************************
 * $Id: tcp.c,v 1.15.2.1 2002/10/28 18:49:49 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Transmission Control Protocol
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
 * tcpInit()
 * tcpCntl()
 * tcpTimer()
 * tcpInput()
 * tcpUser()
 * ngProto_TCP
 *----------------------------------------------------------------------------
 * 10/12/98 - Regis Feneon
 * 29/12/98 -
 *  new function tcpCntl() added
 * 07/04/99 -
 *  NG_PRU_SEND did not return the number of bytes sent
 * 11/04/99 -
 *  do not reuse buffers with already sent data
 * 26/06/99 -
 *  tcpInput:
 *      case TCPS_LISTEN:
 *          initialize congestion window to 1 segment
 *      ACK flag processing, adjusting congestion window:
 *          NGuint type for 'cw' and 'incr' variables
 * 27/06/99 -
 *  tcpInput:
 *      case TCPS_LISTEN:
 *          if PRU_ATTACH failed socket must be linked to itself
 *          so_aq0len increase/decrease correction
 * 10/09/99 -
 *  tcpTimer:
 *      tcb_idle, tcb_rtt was not correctly incremented
 *      ngTcp_Iss now also incremented here
 * 13/09/99 -
 *  tcpInput:
 *  arguments for ngTcpReass() calls have changed
 * 11/10/99 -
 *  tcpUserOption:
 *  cmd argument removed, now use NG_SO_SET/NG_SO_GET
 * 19/10/99 -
 *  tcpUser() - NG_PRU_CONNECT
 *   adjust tcb_maxseg to interface mtu for local connections
 * 29/11/99 -
 *  tcpTimer() - added test for tp not null before incrementing idle and rtt
 * 18/02/2000 - Adrien Felon
 *  tcpInput() - adding argument "flag" for ngIpOutput() call
 * 09/06/2000 - Regis Feneon
 *  cleanup initialization of static data
 * 19/09/2000 -
 *  tcpInit() - initialization of global data
 * 25/09/2000 -
 *  added some code optimization and clarifications
 * 17/11/2000 -
 *  added statistics
 *  removed some compiler warnings
 * 24/11/2000 -
 *  new ngProtoList & NGproto structure
 * 27/11/2000 -
 *  added test for unicast dest address of passive opens
 *  new argument to ngSoLookup()
 * 23/01/2001 -
 *  corrections in NGiphdr, NGtcphdr & NGtcpiphdr structures
 * 18/02/2001 -
 *  added DONTROUTE option
 * 21/02/2001 -
 *  tcpUser() - seprarated NG_PRU_DISCONNECT/NG_PRU_DETACH for linger option
 *              added NG_PRU_RCVOOB & NG_PRU_SENDOOB code
 * 22/02/2001 -
 *  initialize tcb_maxseg to 0 then adjust to the righ value
 *  TCP_MAXSEG now can be used to set the mss before the connection
 * 06/03/2001 -
 *  added NG_TCPO_ROUTE_UPDATE command
 * 12/04/2001 -
 *  header prediction code
 *  added dup-ack and out-of-order packets counters
 * 17/04/2001 -
 *  tcpInput(): check ack number before updating rtt
 *  tcpTimer(): send keepalive probes
 * 28/05/2001 -
 *  global data moved to tcp_data.c
 * 18/04/2002 -
 *  tcpInput(): removed unused variable tbufp
 * 28/10/2002 -
 *  removed some warnings with msvc
 *****************************************************************************/

#include <ngip.h>
#include <ngtcp.h>
#include <ngip/ip.h>
#include <ngip/tcp.h>
#include <ngdsock.h>
#include "rvf_target.h"

#define HEADER_PREDICTION

extern NGtcpcb ngTcpcbTable[];

static NGushort tcpPort; /* ephemeral port */

#ifdef NG_DEBUG
static char dgbuf[1128];
#endif

/*****************************************************************************
 * Internal functions
 *****************************************************************************
 */

/*----------------------------------------------------------------------------
 * tcp_template()
 *----------------------------------------------------------------------------
 * Create TCP/IP header template
 *----------------------------------------------------------------------------
 */
static void tcp_template( NGtcpcb *tp)
{
    tp->tcb_thdr.ti_zero1 = 0;
    tp->tcb_thdr.ti_zero2 = 0;
    tp->tcb_thdr.ti_zero3 = 0;
    tp->tcb_thdr.ti_pr = IPPROTO_TCP;
    /*lint -e572 (Warning -- Excessive shift value) */
    tp->tcb_thdr.ti_len = ngConstHTONS( sizeof( NGtcphdr));
    /*lint +e572 (Warning -- Excessive shift value) */
    tp->tcb_thdr.ti_src = tp->tcb_sock->so_laddr;
    tp->tcb_thdr.ti_dst = tp->tcb_sock->so_faddr;
    tp->tcb_thdr.ti_sport = tp->tcb_sock->so_lport;
    tp->tcb_thdr.ti_dport = tp->tcb_sock->so_fport;
    tp->tcb_thdr.ti_seq = 0;
    tp->tcb_thdr.ti_ack = 0;
    tp->tcb_thdr.ti_x2off = 5<<NG_TCPOFF_SHIFT;
    tp->tcb_thdr.ti_win = 0;
    tp->tcb_thdr.ti_sum = 0;
    tp->tcb_thdr.ti_urp = 0;
}

/*----------------------------------------------------------------------------
 * tcp_usrclosed()
 *----------------------------------------------------------------------------
 * after a process close() call,
 * move connection to next state
 *----------------------------------------------------------------------------
 */
static NGtcpcb *tcp_usrclosed( NGtcpcb *tp)
{
    switch( tp->tcb_state) {
    case TCPS_CLOSED:
    case TCPS_LISTEN:
    case TCPS_SYN_SENT:
        tp->tcb_state = TCPS_CLOSED;
        tp = ngTcpClose( tp);
        break;
    case TCPS_SYN_RECEIVED:
    case TCPS_ESTABLISHED:
        tp->tcb_state = TCPS_FIN_WAIT_1;
        break;
    case TCPS_CLOSE_WAIT:
        tp->tcb_state = TCPS_LAST_ACK;
        break;
    }
    if( tp && (tp->tcb_state >= TCPS_FIN_WAIT_2)) {
        ngSoIsDisconnected( tp->tcb_sock);
    }
    return( tp);
}

#ifdef HEADER_PREDICTION
/*----------------------------------------------------------------------------
 * tcp_snd_drop()
 *----------------------------------------------------------------------------
 * remove acknoledge data from the send buffer
 *----------------------------------------------------------------------------
 */
static void tcp_snd_drop( NGtcpcb *tp, int acked)
{
  int ictl;
  NGbuf *tbufp;
  NGsock *so;

  so = tp->tcb_sock;

  /* remove acked bytes in buffer */
  tp->tcb_buf_ackcc += acked;
  while( tp->tcb_buf_ackcc) {
    tbufp = tp->tcb_buf_waitack;
    if( tbufp == NULL) break;
    if( tbufp->buf_iovcnt <= tp->tcb_buf_ackcc) {
      /* all data in buffer is acknoledged */
      /* so free it */
      tp->tcb_buf_waitack = tbufp->buf_next;
      so->so_snd_cc -= tbufp->buf_iovcnt;
      tp->tcb_buf_ackcc -= tbufp->buf_iovcnt;
      ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE);
      if( tbufp->buf_flags & NG_BUFF_BUSY) {
        tbufp->buf_flags &= ~(NG_BUFF_BUSY|NG_BUFF_DONTFREE);
        ngOSIntrCtl( ictl);
      }
      else {
        ngOSIntrCtl( ictl);
        ngBufFree( tbufp);
      }
    }
    else break;
  }

}
#endif

/*****************************************************************************
 * tcpInit()
 *****************************************************************************
 * Protocol Initialization
 *****************************************************************************
 */

static void tcpInit( void)
{

    /* initialize socket in use list */
    ngTcp_Sockq.next = ngTcp_Sockq.prev = &ngTcp_Sockq;

    /* message buffers */
    ngTcpBufDataOffset = ngBufDataOffset + sizeof( NGiphdr) + sizeof( NGtcphdr);

    /* no tcp control block yet */
    NG_QUEUE_INIT( &ngTcp_TcpcbFreeq, 0);

    /* initialize global data */
    tcpPort = IPPORT_RESERVED;
    ngTcp_Iss = NG_TCP_ISS_INIT;
    ngTcp_LastSock = NULL;

#ifdef NG_IPSTATS_SUPPORTED
    ngMemSet( &ngTcpStat, 0, sizeof( ngTcpStat));
#endif
}

/*****************************************************************************
 * tcpCntl()
 *****************************************************************************
 * Protocol Control
 *****************************************************************************
 */

static int tcpCntl( int cmd, int opt, void *arg)
{
  int i;
  NGtcpcb *tp;
  NGsock *so, *nextso;
  NGifnet *netp;
  NGuint gaddr;

  if( cmd == NG_CNTL_SET) switch( opt) {
  case NG_TCPO_TCB_MAX:
    if( ngTcp_TcpcbFreeq.qu_nelmax) return( NG_EINVAL);
    ngTcp_TcpcbFreeq.qu_nelmax = *((int *) arg);
    return( NG_EOK);
  case NG_TCPO_TCB_TABLE:
    if( !ngTcp_TcpcbFreeq.qu_nelmax) return( NG_EINVAL);
    tp = *((NGtcpcb **) arg);
    for( i=0; i<ngTcp_TcpcbFreeq.qu_nelmax; i++) {
        NG_QUEUE_IN( &ngTcp_TcpcbFreeq, tp);
        tp++;
    }
    return( NG_EOK);

  case NG_TCPO_ROUTE_UPDATE:
    /*
     * close connections where network is down
     */
    so = (NGsock *) ngTcp_Sockq.next;

    while( so != (NGsock *) &ngTcp_Sockq) {
      nextso = (NGsock *) so->so_node.next;
      /* get TCP control block */
      tp = so->so_data;
      if( tp->tcb_state >= NG_TCPS_SYN_SENT) {
        netp = ngIpRouteLookup( so->so_faddr, &gaddr, 0);
        if( (netp == NULL) || !(netp->if_flags & NG_IFF_UP)) {
          /* report error to application */
          if( tp->tcb_softerror)
            so->so_error = tp->tcb_softerror;
          else
            so->so_error = NG_ENETDOWN;
          /* close socket and release resources */
          ngTcpClose( tp);
        }
      }
      so = nextso;
    }

  }
  return( NG_EINVAL);
}

/*****************************************************************************
 * tcpTimer()
 *****************************************************************************
 * TCP Timers processing
 *****************************************************************************
 */

static void tcpTimer( void)
{
  NGsock *so, *nextso;
  NGtcpcb *tp;

  so = (NGsock *) ngTcp_Sockq.next;

  while( so != (NGsock *) &ngTcp_Sockq) {

    nextso = (NGsock *) so->so_node.next;

    /* get TCP control block */
    tp = so->so_data;

    /*
     * Process delayed ACK
     */
    if( tp->tcb_flags & NG_TF_DELACK) {
      tp->tcb_flags &= ~NG_TF_DELACK;
      tp->tcb_flags |= NG_TF_ACKNOW;
      ngTcpOutput( tp);
    }

    /*
     * Updates timers
     */

    /* REXMT */
    if( tp->tcb_tm_rexmt && (--tp->tcb_tm_rexmt == 0)) {
      if( ++tp->tcb_rxtshift > NG_TCP_REXMT_MAXSHIFT) {
        /* maximum retransmission counter reached: drop connection */
        tp->tcb_rxtshift = NG_TCP_REXMT_MAXSHIFT;
        tp = ngTcpDrop( tp, tp->tcb_softerror ? tp->tcb_softerror : NG_ETIMEDOUT);
      }
      else {
        NGuint win;
        int rexmt;

        /* calculate new RTO */
        rexmt = NG_TCP_REXMTVAL( tp)*ngTcp_Backoff[tp->tcb_rxtshift];
        NG_TCPT_RANGESET( tp->tcb_rxtcur , rexmt,
                          tp->tcb_rttmin, NG_TCP_REXMT_MAX);
        tp->tcb_tm_rexmt = tp->tcb_rxtcur;

        /* segment retransmitted at least four times: clear estimators */
        if( tp->tcb_rxtshift > NG_TCP_REXMT_MAXSHIFT/4) {
          tp->tcb_rttvar += (tp->tcb_srtt >> NG_TCP_RTT_SHIFT);
          tp->tcb_srtt = 0;
        }
        /* restransmit unacknowledged data */
        tp->tcb_snd_nxt = tp->tcb_snd_una;

        /* Karn's algorithm */
        tp->tcb_rtt = 0;

        /* congestion avoidance */
        tp->tcb_snd_ssthresh = tp->tcb_maxseg<<1;
        win = NG_MIN( tp->tcb_snd_wnd, tp->tcb_snd_cwnd)>>1;
        if( win > tp->tcb_snd_ssthresh) {
          tp->tcb_snd_ssthresh = win;
        }
        tp->tcb_snd_cwnd = tp->tcb_maxseg; /* force slow start */
        tp->tcb_dupacks = 0;

        /* retransmit segment */
        ngTcpOutput( tp);
      }
    }

    /* PERSIST timer */
    if( tp && (tp->tcb_tm_persist && (--tp->tcb_tm_persist == 0))) {
      ngTcpSetPersist( tp);
      tp->tcb_force = 1;
      ngTcpOutput( tp);
      tp->tcb_force = 0;
    }

    /* KEEPALIVE / Connection Timer */
    if( tp && (tp->tcb_tm_keep && (--tp->tcb_tm_keep == 0))) {
      int dropit;

      dropit = 0;
      if( tp->tcb_state < TCPS_ESTABLISHED) {
        /* connection-establishment timer expiration */
        dropit++;
      }
      else if( (so->so_options & NG_SO_KEEPALIVE) &&
         (tp->tcb_state <= TCPS_CLOSE_WAIT) ) {
        /* keepalive timer */
        if( tp->tcb_idle >= (NG_TCP_KEEP_IDLE + NG_TCP_KEEP_MAXIDLE)) {
          /* max probes without answer, drop connection */
          dropit++;
        }
        else {
          NGbuf *bufp;
          NGtcpiphdr *ti;
          NGuint win;

          /* generate a keepalive probe */
          ngBufAlloc( bufp);
          if( bufp != NULL) {
            bufp->buf_datap = ((NGubyte *) bufp) + ngTcpBufDataOffset;
            bufp->buf_datalen = sizeof( NGtcpiphdr);
            ti = (NGtcpiphdr *) bufp->buf_datap;
            /* copy header template */
            ngMemCpy( ti, &tp->tcb_thdr, sizeof( NGtcpiphdr));
            win = so->so_rcv_hiwat - so->so_rcv_cc;
            /* set TCP header fields */
            ti->ti_flags = TH_ACK;
            ti->ti_win = ngHTONS( (NGushort) win);
            ti->ti_seq = ngHTONL( tp->tcb_snd_una - 1); /* force peer to respond */
            ti->ti_ack = ngHTONL( tp->tcb_rcv_nxt);
            /* TCP checksum */
            ti->ti_sum = ngIpCksum( ti, sizeof( NGtcpiphdr), NULL, 0);
            /* IP header */
            ((NGiphdr *) ti)->ip_hlv = NG_IPVER_IPV4 | (sizeof( NGiphdr)>>2);
            ((NGiphdr *) ti)->ip_tos = so->so_tos;
            ((NGiphdr *) ti)->ip_len = sizeof( NGtcpiphdr);
            ((NGiphdr *) ti)->ip_ttl = so->so_ttl;
#ifdef NG_DEBUG
{
    char *p;
    p = dgbuf;
    p += ngSPrintf( p, "%I:%d > %I:%d ",
        ti->ti_src, ngNTOHS( ti->ti_sport), ti->ti_dst, ngNTOHS( ti->ti_dport));
    if( ti->ti_flags == 0) p += ngSPrintf( p, ".");
    if( ti->ti_flags & TH_FIN) p += ngSPrintf( p, "F");
    if( ti->ti_flags & TH_SYN) p += ngSPrintf( p, "S");
    if( ti->ti_flags & TH_RST) p += ngSPrintf( p, "R");
    if( ti->ti_flags & TH_PUSH) p += ngSPrintf( p, "P");
    if( ti->ti_flags & TH_URG) p += ngSPrintf( p, "U");
    p += ngSPrintf( p, " %lu:%lu (%d) ", ngNTOHL( ti->ti_seq), ngNTOHL( ti->ti_seq) +
          ngNTOHS( ti->ti_len)-(ti->ti_x2off>>2), ngNTOHS( ti->ti_len)-(ti->ti_x2off>>2));
    if( ti->ti_flags & TH_ACK) p += ngSPrintf( p, "ack %lu ", ngNTOHL( ti->ti_ack));
    ngSPrintf( p, "win %d", ngNTOHS( ti->ti_win));
    ngDebug( NG_DBG_TCP, NG_DBG_OUTPUT, 0, "%s", dgbuf);
}
#endif
#ifdef NG_IPSTATS_SUPPORTED
            ngTcpStat.tcps_opackets++;
#endif
            /* send buffer */
            ngIpOutput( bufp, so->so_options & NG_SO_DONTROUTE);
          }

          /* restart timer */
          tp->tcb_tm_keep = NG_TCP_KEEP_INTVL;
        }
      }
      else tp->tcb_tm_keep = NG_TCP_KEEP_IDLE;
      if( dropit) {
        tp = ngTcpDrop( tp, NG_ETIMEDOUT);
      }
    }

    /* 2MSL: FIN_WAIT_2 and TIME_WAIT timers */
    if( tp && (tp->tcb_tm_2msl && (--tp->tcb_tm_2msl == 0))) {
      if( (tp->tcb_state != TCPS_TIME_WAIT) &&
           (tp->tcb_idle <= NG_TCP_KEEP_MAXIDLE) ) {
        tp->tcb_tm_2msl = NG_TCP_KEEP_INTVL;
      }
      else {
        tp = ngTcpClose( tp);
      }
    }

    if( tp) {
      /* increment idle time */
      tp->tcb_idle++;

      /* increment RTT counter */
      if( tp->tcb_rtt) tp->tcb_rtt++;
    }

    /* next socket */
    so = nextso;
  }

  /* increment ISS */
  ngTcp_Iss += NG_TCP_ISS_INCR/NG_TIMERHZ;
}

/*****************************************************************************
 * tcpInput()
 *****************************************************************************
 * Process input messages
 *****************************************************************************
 */

static void tcpInput( NGbuf *bufp)
{
    NGiphdr *ip;
    NGtcphdr *th;
    NGtcpiphdr *ti;
    NGsock *so, *head;
    NGtcpcb *tp;
    NGsockaddr addr;
    NGuint laddr;
    int len, off, tiflags, todrop, win, rcvmss;
    int sendreset, dropsocket;
    int acked, needoutput, ourfinisacked;
    unsigned tiwin;
    NGuint iss;
    NGuint tmpi;
    NGushort tmps;
    int hlen;

    sendreset = 0;
    dropsocket = 0;
    rcvmss = 0;

    iss = 0;
    needoutput = 0;
    tiflags = 0;

#ifdef NG_IPSTATS_SUPPORTED
    ngTcpStat.tcps_ipackets++;
#endif

    /* protocols headers */
    ip = (NGiphdr *) bufp->buf_datap;
    hlen = (ip->ip_hlv & NG_IPHDRLEN_MASK)<<2;
    ip->ip_len -= hlen;
    th = (NGtcphdr *) (((NGubyte *) ip)+hlen);

    /* if there are ip options... strip them */
    if( (unsigned)hlen > sizeof( NGiphdr)) {
        ngMemMove( ((NGubyte *) ip)+(hlen - sizeof( NGiphdr)),
                   ip, sizeof( NGiphdr));
        ip = (NGiphdr *) (((NGubyte *) ip) + (hlen - sizeof( NGiphdr)));
        bufp->buf_datap = (NGubyte *) ip;
        bufp->buf_datalen -= (hlen - sizeof( NGiphdr));
    }
    /* ip data length */
    len = ip->ip_len;

#ifdef NG_IPFRAG_SUPPORTED
    if( bufp->buf_flags & NG_BUFF_MOREDATA) {
      NGubyte *dp;
      NGbuf *tbufp, *tbufp2;
      int remlen;

      /* try to reassemble data if segment not too big */
      if( len > (int) (ngBufDataMax - hlen)) {
#ifdef NG_IPSTATS_SUPPORTED
        ngTcpStat.tcps_ierrors++; /* not really a bad packet but... */
#endif
        /* cant receive this fragmented data */
        while( bufp) {
          /* free all buffers */
          tbufp = bufp->buf_next;
          ngBufFree( bufp);
          bufp = tbufp;
        }
        return;
      }
      remlen = len - bufp->buf_datalen;
      dp = bufp->buf_datap + bufp->buf_datalen;
      tbufp = bufp->buf_next;
      while( tbufp != NULL) {
        /* copy data */
        if( remlen > 0) {
          remlen -= tbufp->buf_datalen;
          if( remlen < 0) {
            /* skip additionnal data */
            tbufp->buf_datalen += remlen;
            remlen = 0;
          }
          /* copy data */
          ngMemCpy( dp, tbufp->buf_datap, tbufp->buf_datalen);
          dp += tbufp->buf_datalen;
        }
        /* release buffer and get next */
        tbufp2 = tbufp->buf_next;
        ngBufFree( tbufp);
        tbufp = tbufp2;
      }
    }
#endif

    /* create pseudo tcp/ip header */
    ti = (NGtcpiphdr *) ip;
    ((NGtcpiphdr *) ip)->ti_zero1 = 0;
    ((NGtcpiphdr *) ip)->ti_zero2 = 0;
    ((NGtcpiphdr *) ip)->ti_zero3 = 0;
    ((NGtcpiphdr *) ip)->ti_len = ngHTONS( (NGushort) len);

    /* test checksum */
    th->th_sum = ngIpCksum( ip, len + sizeof( NGiphdr), NULL, 0);
    if( th->th_sum != 0) {
#ifdef NG_DEBUG
        ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 1, "Bad checksum (%04X)", th->th_sum);
#endif
#ifdef NG_IPSTATS_SUPPORTED
        ngTcpStat.tcps_ierrors++;
#endif
        goto drop;
    }
    /* data offset */
    off = (ti->ti_x2off & NG_TCPOFF_MASK)>>(NG_TCPOFF_SHIFT-2);
    if( ((unsigned)off < sizeof( NGtcphdr)) || (off > len)) {
#ifdef NG_DEBUG
        ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 1, "Bad data offset");
#endif
#ifdef NG_IPSTATS_SUPPORTED
        ngTcpStat.tcps_ierrors++;
#endif
        goto drop;
    }
    len -= off; /* tcp data length */
    ti->ti_len = len;

    tiflags = ti->ti_flags;
    /* process TCP options (just MSS option supported) */
    if( (unsigned)off > sizeof( NGtcphdr)) {
        if( tiflags & TH_SYN) {
            NGubyte *p, *pmax;
            /* just process mss in syn segments */
            p = ((NGubyte *) ip) + sizeof( NGtcpiphdr);
            pmax = p + off - sizeof( NGtcphdr);
            while( p < pmax) {
                if( *p == TCPOPT_NOP) p++;
                else if( *p == TCPOPT_EOL) break;
                else if( *p == TCPOPT_MAXSEG) {
                    p++;
                    if( *p != TCPOPTLEN_MAXSEG) break;
                    p++;
                    ngMemCpy( &tmps, p, sizeof( NGushort));
                    rcvmss = ngNTOHS( tmps);
                    break;
                }
                else {
                    p++;
                    p += (*p - 2);
                }
            }
        }
    }

    /* convert TCP fields to host format */
    ti->ti_seq = ngNTOHL( ti->ti_seq);
    ti->ti_ack = ngNTOHL( ti->ti_ack);
    ti->ti_win = ngNTOHS( ti->ti_win);
    ti->ti_urp = ngHTONS( ti->ti_urp);

#ifdef NG_DEBUG
{
    char *p;

    p = dgbuf;
    p += ngSPrintf( p, "%I:%d < %I:%d ",
        ip->ip_dst, ngNTOHS( th->th_dport), ip->ip_src, ngNTOHS( th->th_sport));
    if( tiflags == 0) p += ngSPrintf( p, ".");
    if( tiflags & TH_FIN) p += ngSPrintf( p, "F");
    if( tiflags & TH_SYN) p += ngSPrintf( p, "S");
    if( tiflags & TH_RST) p += ngSPrintf( p, "R");
    if( tiflags & TH_PUSH) p += ngSPrintf( p, "P");
    if( tiflags & TH_URG) p += ngSPrintf( p, "U");
    p += ngSPrintf( p, " %lu:%lu (%d) ", ti->ti_seq, ti->ti_seq+ti->ti_len, ti->ti_len);
    if( tiflags & TH_ACK) p += ngSPrintf( p, "ack %lu ", ti->ti_ack);
    p += ngSPrintf( p, "win %d", ti->ti_win);
    if( rcvmss) ngSPrintf( p, " <mss> %d", rcvmss);
    ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 0, "%s", dgbuf);
}
#endif

    /*
     * Find corresponding socket
     */
findpcb:
    /* search for last used socket first */
    if( ngTcp_LastSock &&
       (ngTcp_LastSock->so_lport == ti->ti_dport) &&
       (ngTcp_LastSock->so_fport == ti->ti_sport) &&
       (ngTcp_LastSock->so_faddr == ti->ti_src) &&
       (ngTcp_LastSock->so_laddr == ti->ti_dst) )
        so = ngTcp_LastSock;
    else so = ngSoLookup( (NGsock *) &ngTcp_Sockq,
     ti->ti_dst, ti->ti_dport, ti->ti_src, ti->ti_sport, 1);

    if( so == NULL) {
#ifdef NG_DEBUG
        ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 0, "No match");
#endif
        sendreset++;
        goto drop_unlock;
    }

    /*
     * Socket found
     */
    ngTcp_LastSock = so;

    /* get tcp control block */
    tp = so->so_data;

    if( (tp == NULL) || (tp->tcb_state == TCPS_CLOSED)) goto drop_unlock;

    tiwin = ti->ti_win;

/*----------------------------------------------------------------------------
 * Accept a new connection
 *----------------------------------------------------------------------------*/
    if( so->so_options & NG_SO_ACCEPTCONN) {
        NGsock *head;

        /* don't accept more than aqmax connections */
        if( (so->so_aq0len + so->so_aqlen) >= so->so_aqmax) {
#ifdef NG_DEBUG
            ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 0, "Accept queue full (%d %d %d)",
                         so->so_aq0len, so->so_aqlen, so->so_aqmax);
#endif
            goto drop_unlock;
        }

        /*
         * Create temporary socket with same properties
         */
        head = so;
        so = ngSoAlloc( head);
        if( so == NULL) {
#ifdef NG_DEBUG
            ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 0, "Cant allocate new socket");
#endif
            goto drop_unlock;
        }
        dropsocket++;
        so->so_options &= ~NG_SO_ACCEPTCONN;
        so->so_state |= NG_SS_NOFDREF;
        so->so_laddr = ti->ti_dst;
        so->so_lport = ti->ti_dport;
        if( NG_SOCKCALL_USER( so, NG_PRU_ATTACH, NULL, 0, NULL)) {
#ifdef NG_DEBUG
            ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 0, "Cant attach new socket");
#endif
            so->so_head = NULL;
            /* socket is unlinked 27/6/99 */
            so->so_node.next = so->so_node.prev = (NGnode *) so;
            goto drop_unlock;
        }
        /* adds socket in head's accept list */
        while( head->so_acceptq0) head = head->so_acceptq0;
        head->so_acceptq0 = so;
        so->so_head->so_aq0len++; /* 27/6/99 */

        tp = so->so_data;
        tp->tcb_state = TCPS_LISTEN;

    }

    /* reset idle time and keepalive timer */
    tp->tcb_idle = 0;
    tp->tcb_tm_keep = NG_TCP_KEEP_IDLE;

#ifdef HEADER_PREDICTION
/*----------------------------------------------------------------------------
 * Header prediction
 *----------------------------------------------------------------------------*/
  if( tp->tcb_state == TCPS_ESTABLISHED &&
   ((tiflags & (TH_SYN|TH_FIN|TH_RST|TH_URG|TH_ACK)) == TH_ACK) &&
   (ti->ti_seq == tp->tcb_rcv_nxt) && tiwin && (tiwin == tp->tcb_snd_wnd) &&
   (tp->tcb_snd_nxt == tp->tcb_snd_max) ) {

    /* test for pure ACK */
    if( ti->ti_len == 0) {

      if( NG_SEQ_GT( ti->ti_ack, tp->tcb_snd_una) &&
          NG_SEQ_LE( ti->ti_ack, tp->tcb_snd_max) &&
          (tp->tcb_snd_cwnd >= tp->tcb_snd_wnd)) {

        /* update rtt estimator */
        if( tp->tcb_rtt && NG_SEQ_GT( ti->ti_ack, tp->tcb_rtseq))
          ngTcpXmitTimer( tp, tp->tcb_rtt);

        /* remove acked data from buffer */
        acked = ti->ti_ack - tp->tcb_snd_una;
        tcp_snd_drop( tp, acked);
        tp->tcb_snd_una = ti->ti_ack;
        ngBufFree( bufp);

        /* stop retransmit timer if no more data waiting */
        if( tp->tcb_snd_una == tp->tcb_snd_max)
          tp->tcb_tm_rexmt = 0;
        else if( !tp->tcb_tm_persist)
          tp->tcb_tm_rexmt = tp->tcb_rxtcur; /* restart timer */

        /* wake up sleeping processes */
        ngSoDataSent( so);

        /* send more data */
        if( so->so_snd_cc) ngTcpOutput( tp);

        return;
      }

    } /* ti_len == 0 */

    /* test for data in sequence */
    else if( (ti->ti_ack == tp->tcb_snd_una) &&
             (tp->tcb_reassq.next == &tp->tcb_reassq) &&
             (ti->ti_len <= (so->so_rcv_hiwat - so->so_rcv_cc))) {

      tp->tcb_rcv_nxt += ti->ti_len;

      /* drop ip and tcp headers */
      bufp->buf_datap += sizeof( NGiphdr) + off;
      bufp->buf_datalen -= sizeof( NGiphdr) + off;

      {
        NGbuf *tbufp;

        /* last receive buffer */
        tbufp = (NGbuf *) so->so_rcv_q.qu_tail;
        /* does the data may be append to last buffer ? */
        if( tbufp &&
         ((tbufp->buf_datap + tbufp->buf_datalen + bufp->buf_datalen) <
         (((NGubyte *) tbufp) + ngTcpBufDataOffset + tp->tcb_maxseg))) {
          /* copy the data in last buffer */
          ngMemCpy( tbufp->buf_datap + tbufp->buf_datalen,
                    bufp->buf_datap, bufp->buf_datalen);
          tbufp->buf_datalen += bufp->buf_datalen;
          ngBufFree( bufp);
        }
        else {
          /* queue buffer in socket receive buffer */
          NG_QUEUE_IN( &so->so_rcv_q, bufp);
        }
        /* update receive byte count */
        so->so_rcv_cc += bufp->buf_datalen;

        /* wake up awaiting process */
        ngSoDataReceived( so);
      }

      tp->tcb_flags |= NG_TF_DELACK;
      return;

    }

  }
#endif

/*----------------------------------------------------------------------------
 * Slow path processing
 *----------------------------------------------------------------------------*/

    /* drop ip and tcp headers */
    bufp->buf_datap += sizeof( NGiphdr) + off;
    bufp->buf_datalen -= sizeof( NGiphdr) + off;

    /* calculate amount of space in receive window */
    win = so->so_rcv_hiwat - so->so_rcv_cc;
    if( win < 0) win = 0;
    tp->tcb_rcv_wnd = NG_MAX( win, (int) (tp->tcb_rcv_adv - tp->tcb_rcv_nxt));

    switch( tp->tcb_state) {

/*----------------------------------------------------------------------------
 * LISTEN
 * Initiation of Passive Open
 *----------------------------------------------------------------------------*/
    case TCPS_LISTEN:

        /* drop if RST, ACK or no SYN */
        if( tiflags & TH_RST) goto drop_unlock;
        if( tiflags & TH_ACK) {
            sendreset++;
            goto drop_unlock;
        }
        if( (tiflags & TH_SYN) == 0) goto drop_unlock;
        /* dest address must be unicast */
        if( (ti->ti_dst == NG_INADDR_BROADCAST) ||
            NG_IN_MULTICAST( ti->ti_dst) ) {
            goto drop_unlock;
        }
        /* test for broadcast addresses */
        {
            NGifnet *netp;
            netp = ngIfList;
            while( netp) {
              if( (netp->if_flags & NG_IFF_BROADCAST) &&
                  (netp->if_addr) &&
                  ((netp->if_netbroadcast == ti->ti_dst) ||
                   (netp->if_broadaddr == ti->ti_dst)) )
              goto drop_unlock;
              netp = netp->if_next;
            }
        }
        /* connect socket to peer */
        laddr = so->so_laddr;
        if( so->so_laddr == INADDR_ANY) so->so_laddr = ti->ti_dst;
        addr.sin_addr = ti->ti_src;
        addr.sin_port = ti->ti_sport;
        if( ngSoConnect( so, &addr, (NGsock *) &ngTcp_Sockq, &tcpPort)) {
            so->so_laddr = laddr;
            goto drop_unlock;
        }
        /* initialize tcp/ip header template */
        tcp_template( tp);

        /* find interface and adjust mss... */
        /* ...if not already set - 22/02/2001 */
        if( tp->tcb_maxseg == 0) {
          NGifnet *netp;
          NGuint gaddr;

          netp = ngIpRouteLookup( tp->tcb_sock->so_faddr, &gaddr,
                    so->so_options & NG_IPOUTPUT_DONTROUTE);
          /* if destination host is directly attached to the interface */
          /* take interface mtu as mss */
          if( netp && (tp->tcb_sock->so_faddr == gaddr))
              tp->tcb_maxseg = netp->if_mtu - sizeof( NGtcpiphdr);
          else
            tp->tcb_maxseg = NG_TCP_MSS;
        }

        /* cannot have maxseg > bufdatamax */
        if( tp->tcb_maxseg > (int) (ngBufDataMax - sizeof( NGtcpiphdr)))
          tp->tcb_maxseg = ngBufDataMax - sizeof( NGtcpiphdr);

        /* DO tcp options */
        if( rcvmss) tp->tcb_maxseg = NG_MIN( tp->tcb_maxseg, rcvmss);

        /* initialize congestion window to 1 segment */
        tp->tcb_snd_cwnd = tp->tcb_maxseg;

        /* initialize sequence numbers */
        if( iss) tp->tcb_iss = iss;
        else tp->tcb_iss = ngTcp_Iss;
        ngTcp_Iss += NG_TCP_ISS_INCR;
        tp->tcb_irs = ti->ti_seq;
        tp->tcb_rcv_adv = tp->tcb_rcv_nxt = tp->tcb_irs + 1;
        tp->tcb_snd_una = tp->tcb_snd_nxt = tp->tcb_snd_max = tp->tcb_snd_up = tp->tcb_iss;
        /* ACK the SYN and update state */
        tp->tcb_flags |= NG_TF_ACKNOW;
        tp->tcb_state = TCPS_SYN_RECEIVED;
        tp->tcb_tm_keep = NG_TCP_KEEP_INIT;

        /* dropsocket = 0; */
#ifdef NG_DEBUG
        ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 0, "Accepted!");
#endif
#ifdef NG_IPSTATS_SUPPORTED
        ngTcpStat.tcps_accepts++;
#endif
        goto trimthenstep6;

/*----------------------------------------------------------------------------
 * SYN_SENT
 * Completion of Active Open
 *----------------------------------------------------------------------------*/
    case TCPS_SYN_SENT:
        /* verify received ACK */
        if( (tiflags & TH_ACK) && ( NG_SEQ_LE( ti->ti_ack, tp->tcb_iss) ||
             NG_SEQ_GT( ti->ti_ack, tp->tcb_snd_max) ) ) {
            sendreset++;
            goto drop_unlock;
        }
        /* process and drop RST segment */
        if( tiflags & TH_RST) {
            if( tiflags & TH_ACK) {
                /* tp = */ ngTcpDrop( tp, NG_ECONNREFUSED);
            }
            goto drop_unlock;
        }
        /* verify SYN flag set */
        if( (tiflags & TH_SYN) == 0) goto drop_unlock;
        /* process ACK */
        if( tiflags & TH_ACK) {
            tp->tcb_snd_una = ti->ti_ack;
            if( NG_SEQ_LT( tp->tcb_snd_nxt, tp->tcb_snd_una))
                tp->tcb_snd_nxt = tp->tcb_snd_una;
            tp->tcb_tm_rexmt = 0; /* turn off timer */
        }
        /* initialize sequence numbers */
        tp->tcb_irs = ti->ti_seq;
        tp->tcb_rcv_adv = tp->tcb_rcv_nxt = tp->tcb_irs + 1;
        tp->tcb_flags |= NG_TF_ACKNOW;

        if( tiflags & TH_ACK) {
            /* connection complete */
#ifdef NG_DEBUG
            ngDebug( NG_DBG_TCP, NG_DBG_INPUT, 1, "Connection complete!");
#endif

            /* socket is connected */
            ngSoIsConnected( so);
            tp->tcb_state = TCPS_ESTABLISHED;
            ngTcpReass( tp, 0, 0, NULL);
            if( tp->tcb_rtt)
                ngTcpXmitTimer( tp, tp->tcb_rtt);
        }
        else {
            /* simultaneous open */
            tp->tcb_state = TCPS_SYN_RECEIVED;
        }

        /* DO tcp options */
        if( rcvmss) tp->tcb_maxseg = NG_MIN( tp->tcb_maxseg, rcvmss);

trimthenstep6:
        ti->ti_seq++;
        /* drop data that follows receive window */
        if( ti->ti_len > tp->tcb_rcv_wnd) {
            bufp->buf_datalen -= (ti->ti_len - tp->tcb_rcv_wnd);
            ti->ti_len = tp->tcb_rcv_wnd;
            tiflags &= ~TH_FIN;
        }
        /* force update of window variables */
        tp->tcb_snd_wl1 = ti->ti_seq - 1;
        tp->tcb_rcv_up = ti->ti_seq;
        goto step6;
    }

    /* trim segment */
    todrop = tp->tcb_rcv_nxt - ti->ti_seq;
    if( todrop > 0) {
        /* remove duplicate SYN */
        if( tiflags & TH_SYN) {
            tiflags &= ~TH_SYN;
            ti->ti_seq++;
            if( ti->ti_urp > 1) ti->ti_urp--;
            else tiflags &= ~TH_URG;
            todrop--;
        }
        /* check for entire duplicate packet */
        if( todrop > ti->ti_len ||
            ((todrop == ti->ti_len) && ((tiflags & TH_FIN) == 0)) ) {
            /* drop FIN */
            tiflags &= ~TH_FIN;

            tp->tcb_flags |= NG_TF_ACKNOW;
            todrop = ti->ti_len;
        }
        /* remove duplicate data and update urgent pointer */
        bufp->buf_datap += todrop;
        bufp->buf_datalen -= todrop;
        ti->ti_seq += todrop;
        ti->ti_len -= todrop;
        if( ti->ti_urp > todrop) {
            ti->ti_urp -= todrop;
        }
        else {
            tiflags &= ~TH_URG;
            ti->ti_urp = 0;
        }
    }

    /* send RST if data arrive after socket close */
    if( (so->so_state & NG_SS_NOFDREF) && (tp->tcb_state > TCPS_CLOSE_WAIT)
        && ti->ti_len) {
        /* tp = */ ngTcpClose( tp);
        sendreset++;
        goto drop_unlock;
    }

    /* if segment ends after window, drop trailing data */
    todrop = (ti->ti_seq + ti->ti_len) - (tp->tcb_rcv_nxt + tp->tcb_rcv_wnd);
    if( todrop > 0) {
        if( todrop >= ti->ti_len) {
            /*
             * new connection request is received
             * while in TIME_WAIT
             */
             if( (tiflags & TH_SYN) &&
                 NG_SEQ_GT( ti->ti_seq, tp->tcb_rcv_nxt)) {
                iss = tp->tcb_rcv_nxt = NG_TCP_ISS_INCR;
                /* tp = */ ngTcpClose( tp);
/***************************************************/
                goto findpcb; /* XXX */
/***************************************************/
            }

            if( (tp->tcb_rcv_wnd == 0) && (ti->ti_seq == tp->tcb_rcv_nxt)) {
                tp->tcb_flags |= NG_TF_ACKNOW;
            }
            else goto dropafterack;
        }
        bufp->buf_datalen -= todrop;
        ti->ti_len -= todrop;
        tiflags &= ~(TH_PUSH|TH_FIN);
    }

/*----------------------------------------------------------------------------
 * RST flag processing
 *----------------------------------------------------------------------------*/
    if( tiflags & TH_RST) switch( tp->tcb_state) {
    case TCPS_SYN_RECEIVED:
    case TCPS_ESTABLISHED:
    case TCPS_FIN_WAIT_1:
    case TCPS_FIN_WAIT_2:
    case TCPS_CLOSE_WAIT:
        if( tp->tcb_state == TCPS_SYN_RECEIVED) {
          so->so_error = NG_ECONNREFUSED;
#ifdef NG_IPSTATS_SUPPORTED
          ngTcpStat.tcps_conndrops++;
#endif
        }
        else {
          so->so_error = NG_ECONNRESET;
#ifdef NG_IPSTATS_SUPPORTED
          ngTcpStat.tcps_drops++;
#endif
        }
        tp->tcb_state = TCPS_CLOSED;
        /* tp = */ ngTcpClose( tp);
        goto drop_unlock;
    case TCPS_CLOSING:
    case TCPS_LAST_ACK:
    case TCPS_TIME_WAIT:
        /* tp = */ ngTcpClose( tp);
        goto drop_unlock;
    }

    /* error if SYN in window, send RST and drop connection */
    if( tiflags & TH_SYN) {
        /* tp = */ ngTcpDrop( tp, NG_ECONNRESET);
        sendreset++;
        goto drop_unlock;
    }
    /* if ACK is off drop segment */
    if( (tiflags & TH_ACK) == 0) goto drop_unlock;

/*----------------------------------------------------------------------------
 * ACK flag processing
 *----------------------------------------------------------------------------*/
    switch( tp->tcb_state) {
    case TCPS_SYN_RECEIVED:
        if( NG_SEQ_GT( tp->tcb_snd_una, ti->ti_ack) ||
            NG_SEQ_GT( ti->ti_ack, tp->tcb_snd_max)) {
            sendreset++;
            goto drop_unlock;
        }
        ngSoIsConnected( so);
        tp->tcb_state = TCPS_ESTABLISHED;
        ngTcpReass( tp, 0, 0, NULL);
        tp->tcb_snd_wl1 = ti->ti_seq - 1;
        /* fall into... */
    case TCPS_ESTABLISHED:
    case TCPS_FIN_WAIT_1:
    case TCPS_FIN_WAIT_2:
    case TCPS_CLOSE_WAIT:
    case TCPS_CLOSING:
    case TCPS_LAST_ACK:
    case TCPS_TIME_WAIT:
        if( NG_SEQ_LE( ti->ti_ack, tp->tcb_snd_una)) {
            if( (ti->ti_len == 0) && (tiwin == tp->tcb_snd_wnd)) {
                /* duplicate ack */
#ifdef NG_IPSTATS_SUPPORTED
                ngTcpStat.tcps_idupacks++;
#endif
                if( (tp->tcb_tm_rexmt == 0) ||
                    ti->ti_ack != tp->tcb_snd_una)
                    tp->tcb_dupacks = 0;
                else if( ++tp->tcb_dupacks == NG_TCP_REXMT_THRESH) {
                    NGuint onxt, win;

                    onxt = tp->tcb_snd_nxt;
/*
                    win = NG_MIN( tp->tcb_snd_wnd, tp->tcb_snd_cwnd)/(tp->tcb_maxseg<<1);
                    if( win < 2) win = 2;
                    tp->tcb_snd_ssthresh = win*tp->tcb_maxseg;
*/
                    tp->tcb_snd_ssthresh = tp->tcb_maxseg<<1;
                    win = NG_MIN( tp->tcb_snd_wnd, tp->tcb_snd_cwnd)>>1;
                    if (win > tp->tcb_snd_ssthresh) {
                        tp->tcb_snd_ssthresh = win;
                    }
                    tp->tcb_tm_rexmt = 0;
                    tp->tcb_rtt = 0;
                    tp->tcb_snd_nxt = ti->ti_ack;
                    tp->tcb_snd_cwnd = tp->tcb_maxseg;
                    ngTcpOutput( tp);
                    tp->tcb_snd_cwnd = tp->tcb_snd_ssthresh +
                                       tp->tcb_maxseg*tp->tcb_dupacks;
                    if( NG_SEQ_GT( onxt, tp->tcb_snd_nxt))
                        tp->tcb_snd_nxt = onxt;
                    goto drop_unlock;
                }
                else if( tp->tcb_dupacks > NG_TCP_REXMT_THRESH) {
                    tp->tcb_snd_cwnd += (NGuint) tp->tcb_maxseg;
                    ngTcpOutput( tp);
                    goto drop_unlock;
                }
            }
            else tp->tcb_dupacks = 0;
            break; /* go to step6 */
        }
        /* adjust congestion window */
        if( (tp->tcb_dupacks > NG_TCP_REXMT_THRESH) &&
            (tp->tcb_snd_cwnd > tp->tcb_snd_ssthresh))
            tp->tcb_snd_cwnd = tp->tcb_snd_ssthresh;
        tp->tcb_dupacks = 0;
        /* check for out-of-range ack */
        if( NG_SEQ_GT( ti->ti_ack, tp->tcb_snd_max)) {
            goto dropafterack;
        }
        /* number of bytes acknoledged */
        acked = ti->ti_ack - tp->tcb_snd_una;
        if( tp->tcb_rtt && NG_SEQ_GT( ti->ti_ack, tp->tcb_rtseq)) /* 17/04/2001 */
          ngTcpXmitTimer( tp, tp->tcb_rtt);
        /* check if all outstanding data is acknoldeged */
        if( ti->ti_ack == tp->tcb_snd_max) {
            tp->tcb_tm_rexmt = 0;
            needoutput = 1;
        }
        else if( tp->tcb_tm_persist == 0)
            tp->tcb_tm_rexmt = tp->tcb_rxtcur;
        /* update congestion window */
        {
            NGuint cw, incr;

            cw = tp->tcb_snd_cwnd;
            incr = tp->tcb_maxseg;

            if( cw > tp->tcb_snd_ssthresh)
                incr = incr*incr/cw + (incr>>3);
            tp->tcb_snd_cwnd = NG_MIN( cw+incr, TCP_MAXWIN);
        }

        /* remove acknoledge data from the send buffer */
        if( acked > so->so_snd_cc) {
            tp->tcb_snd_wnd -= so->so_snd_cc;
            /* remove all bytes in buffer */
            NG_TCPBUF_FREE( tp);
            so->so_snd_cc = 0;
            tp->tcb_buf_ackcc = 0;
            ourfinisacked = 1;
        }
        else {
#ifdef HEADER_PREDICTION
            tcp_snd_drop( tp, acked);
#else
            NGbuf *tbufp;
            int ictl;
            /* remove acked bytes in buffer */
            tp->tcb_buf_ackcc += acked;
            while( tp->tcb_buf_ackcc) {
                tbufp = tp->tcb_buf_waitack;
                if( tbufp == NULL) break;
                if( tbufp->buf_iovcnt <= tp->tcb_buf_ackcc) {
                    /* all data in buffer is acknoledged */
                    /* so free it */
                    tp->tcb_buf_waitack = tbufp->buf_next;
                    so->so_snd_cc -= tbufp->buf_iovcnt;
                    tp->tcb_buf_ackcc -= tbufp->buf_iovcnt;
                    ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE);
                    if( tbufp->buf_flags & NG_BUFF_BUSY) {
                        tbufp->buf_flags &= ~(NG_BUFF_BUSY|NG_BUFF_DONTFREE);
                        ngOSIntrCtl( ictl);
                    }
                    else {
                        ngOSIntrCtl( ictl);
                        ngBufFree( tbufp);
                    }
                }
                else break;
            }
#endif
            tp->tcb_snd_wnd -= acked;
            ourfinisacked = 0;
        }
        /* wake up sleeping processes */
        //ngSoDataSent( so);

        tp->tcb_snd_una = ti->ti_ack;
        if( NG_SEQ_LT( tp->tcb_snd_nxt, tp->tcb_snd_una))
            tp->tcb_snd_nxt = tp->tcb_snd_una;
        ngSoDataSent( so);

        switch( tp->tcb_state) {
        /* receipt of ACK in FIN_WAIT_1 state */
        case TCPS_FIN_WAIT_1:
            if( ourfinisacked) {
                if( so->so_state & NG_SS_CANTRCVMORE) {
                    ngSoIsDisconnected( so);
                    tp->tcb_tm_2msl = NG_TCP_KEEP_MAXIDLE;
                }
                tp->tcb_state = TCPS_FIN_WAIT_2;
            }
            break;
        case TCPS_CLOSING:
            if( ourfinisacked) {
                tp->tcb_state = TCPS_TIME_WAIT;
                tp->tcb_tm_rexmt = 0;
                tp->tcb_tm_persist = 0;
                tp->tcb_tm_keep = 0;
                tp->tcb_tm_2msl = 2*NG_TCP_MSL;
                ngSoIsDisconnected( so);
            }
            break;
        case TCPS_LAST_ACK:
            if( ourfinisacked) {
                /* tp = */ ngTcpClose( tp);
                goto drop_unlock;
            }
            break;
        case TCPS_TIME_WAIT:
            tp->tcb_tm_2msl = 2*NG_TCP_MSL;
            goto dropafterack;
        }
    }

step6:
    /*
     * update window information
     */
    if( (tiflags & TH_ACK) && ( NG_SEQ_LT( tp->tcb_snd_wl1, ti->ti_seq) ||
        ( (tp->tcb_snd_wl1 == ti->ti_seq) &&
          ( NG_SEQ_LT( tp->tcb_snd_wl2, ti->ti_ack) ||
          ((tp->tcb_snd_wl2 == ti->ti_ack) && (tiwin > tp->tcb_snd_wnd)) ) )
        ) ) {

        tp->tcb_snd_wnd = tiwin;
        tp->tcb_snd_wl1 = ti->ti_seq;
        tp->tcb_snd_wl2 = ti->ti_ack;
        if( tp->tcb_snd_wnd > tp->tcb_max_sndwnd)
            tp->tcb_max_sndwnd = tp->tcb_snd_wnd;
        needoutput = 1;
    }

/*----------------------------------------------------------------------------
 * URG flag processing
 *----------------------------------------------------------------------------*/
    if( (tiflags & TH_URG) && ti->ti_urp && (tp->tcb_state < TCPS_TIME_WAIT)) {

        /* XXX */
        if( ti->ti_urp + so->so_rcv_cc > so->so_rcv_hiwat) {
            ti->ti_urp = 0;
            tiflags &= ~TH_URG;
        }
        else if( NG_SEQ_GT( ti->ti_seq + ti->ti_urp, tp->tcb_rcv_up)) {
            tp->tcb_rcv_up = ti->ti_seq + ti->ti_urp;
            so->so_oobmark = so->so_rcv_cc + (tp->tcb_rcv_up - tp->tcb_rcv_nxt) -1;
            if( so->so_oobmark == 0)
                so->so_state |= NG_SS_RCVATMARK;

            /* wake up process ? */
            ngSoDataReceived( so);

            tp->tcb_oobflags &= ~(NG_TCPOOB_HAVEDATA|NG_TCPOOB_HADDATA);
        }
        /* remove out-of-band data from normal data stream */
        if( (tiflags & TH_URG) && (ti->ti_urp <= ti->ti_len) &&
            (so->so_options & NG_SO_OOBINLINE) == 0) {
            /* store urgent byte */
            tp->tcb_iobc = *(bufp->buf_datap + ti->ti_urp - 1);
            tp->tcb_oobflags |= NG_TCPOOB_HAVEDATA;
            /* drop it from normal stream */
            ngMemMove( bufp->buf_datap + ti->ti_urp - 1,
                       bufp->buf_datap + ti->ti_urp,
                       bufp->buf_datalen - ti->ti_urp);
            bufp->buf_datalen--;
        }
    }
    else {
        /* no out-of-band data expected... */
        /* pull urgent pointer along with receive window */
        if( NG_SEQ_GT( tp->tcb_rcv_nxt, tp->tcb_rcv_up))
            tp->tcb_rcv_up = tp->tcb_rcv_nxt;
    }

/*----------------------------------------------------------------------------
 * DATA processing
 *----------------------------------------------------------------------------*/
    if( (ti->ti_len || (tiflags & TH_FIN)) && (tp->tcb_state < TCPS_TIME_WAIT)) {
        /* TCP_REASS */
        if( (ti->ti_seq == tp->tcb_rcv_nxt) &&
            ( tp->tcb_reassq.next == &tp->tcb_reassq) &&
            (tp->tcb_state == TCPS_ESTABLISHED)) {
            /* the data is in order */
            tp->tcb_flags |= NG_TF_DELACK;
            tp->tcb_rcv_nxt += ti->ti_len;
            tiflags = ti->ti_flags & TH_FIN;

            if( bufp->buf_datalen) {
                NGbuf *tbufp;

                /* last receive buffer */
                tbufp = (NGbuf *) so->so_rcv_q.qu_tail;
                /* does the data may be append to last buffer ? */
                if( tbufp &&
                 ((tbufp->buf_datap + tbufp->buf_datalen + bufp->buf_datalen) <
                 (((NGubyte *) tbufp) + ngTcpBufDataOffset + tp->tcb_maxseg))) {
                    /* copy the data in last buffer */
                    ngMemCpy( tbufp->buf_datap + tbufp->buf_datalen,
                              bufp->buf_datap, bufp->buf_datalen);
                    tbufp->buf_datalen += bufp->buf_datalen;
                    ngBufFree( bufp);
                }
                else {
                    /* queue buffer in socket receive buffer */
                    NG_QUEUE_IN( &so->so_rcv_q, bufp);
                }
                /* update receive byte count */
                so->so_rcv_cc += bufp->buf_datalen;

                /* wake up awaiting process */
                ngSoDataReceived( so);
            }
            else ngBufFree( bufp);
        }
        else {
#ifdef NG_IPSTATS_SUPPORTED
            ngTcpStat.tcps_ioopackets++;
#endif
            /* out of order data */
            bufp->buf_datalen = ti->ti_len; /* buffer points to data only */
            tiflags = ngTcpReass( tp, ti->ti_seq, ti->ti_flags, bufp);
            tp->tcb_flags |= NG_TF_ACKNOW;
        }

    }
    else {
        ngBufFree( bufp);
        tiflags &= ~TH_FIN;
    }

/*----------------------------------------------------------------------------
 * FIN flag processing
 *----------------------------------------------------------------------------*/
    if( tiflags & TH_FIN) {
        if( tp->tcb_state < TCPS_TIME_WAIT) {
            ngSoCantRcvMore( so);
            tp->tcb_flags |= NG_TF_ACKNOW;
            tp->tcb_rcv_nxt++;
        }
        switch( tp->tcb_state) {
        case TCPS_SYN_RECEIVED:
        case TCPS_ESTABLISHED:
            tp->tcb_state = TCPS_CLOSE_WAIT;
            break;
        case TCPS_FIN_WAIT_1:
            tp->tcb_state = TCPS_CLOSING;
            break;
        case TCPS_FIN_WAIT_2:
            tp->tcb_state = TCPS_TIME_WAIT;
            tp->tcb_tm_rexmt = 0;
            tp->tcb_tm_persist = 0;
            tp->tcb_tm_keep = 0;
            tp->tcb_tm_2msl = 2*NG_TCP_MSL;
            ngSoIsDisconnected( so);
            break;
        case TCPS_TIME_WAIT:
            tp->tcb_tm_2msl = 2*NG_TCP_MSL;
            break;
        }

    }

    /*
     * return any desired output
     */
    if( needoutput || (tp->tcb_flags & NG_TF_ACKNOW))
        ngTcpOutput( tp);

    return;

/*----------------------------------------------------------------------------*/

dropafterack:
    /*
     * generate an ACK dropping incoming segment
     */
    if( !(tiflags & TH_RST)) {
        ngBufFree( bufp);
        tp->tcb_flags |= NG_TF_ACKNOW;
        ngTcpOutput( tp);
        return;
    }

drop_unlock:
    /*
     * free temporary socket ?
     */
    if( so != NULL && dropsocket) {
        /* detach from accept queue */
        if( so->so_head) {
            head = so->so_head;
            while( (head->so_acceptq0 != so) && head->so_acceptq0) head = head->so_acceptq0;
            if( head->so_acceptq0) {
                head->so_acceptq0 = so->so_acceptq0;
                so->so_head->so_aq0len--; /* 27/6/99 */
            }
        }
        so->so_head = NULL;
        tp = so->so_data;
        if( tp) NG_QUEUE_IN( &ngTcp_TcpcbFreeq, tp);
        so->so_data = NULL;
        NG_NODE_DETACH( so);
        ngSoFree( so);
    }
drop:
    /*
     * reply RST to the sender ?
     */
    if( sendreset && !(tiflags & TH_RST) && !(bufp->buf_flags & NG_BUFF_BCAST)) {
        /* generate reply */
        bufp->buf_datap = (NGubyte *) ti;
        bufp->buf_datalen = sizeof( NGtcpiphdr);
        /* exchange source and destination addresses */
        tmpi = ti->ti_src;
        ti->ti_src = ti->ti_dst;
        ti->ti_dst = tmpi;
        tmps = ti->ti_sport;
        ti->ti_sport = ti->ti_dport;
        ti->ti_dport = tmps;
        /* set other fields */
        if( tiflags & TH_ACK) {
            ti->ti_flags = TH_RST;
            ti->ti_seq = ngHTONL( ti->ti_ack);
            ti->ti_ack = 0;
        }
        else {
            if( tiflags & TH_SYN) ti->ti_len++; /* connection rejected */
            ti->ti_flags = TH_RST|TH_ACK;
            ti->ti_ack = ngHTONL( ti->ti_seq + ti->ti_len);
            ti->ti_seq = 0;
        }
        /*lint -e572 (Warning -- Excessive shift value) */
        ti->ti_len = ngHTONS( sizeof( NGtcphdr));
        /*lint +e572 (Warning -- Excessive shift value) */
        ti->ti_x2off = sizeof( NGtcphdr)<<(NG_TCPOFF_SHIFT-2);
        ti->ti_win = 0;
        ti->ti_urp = 0;
        ti->ti_sum = 0;
        ti->ti_sum = ngIpCksum( ti, sizeof( NGtcpiphdr), NULL, 0);
        ((NGiphdr *) ti)->ip_len = sizeof( NGtcpiphdr);
        ((NGiphdr *) ti)->ip_ttl = MAXTTL;
        ((NGiphdr *) ti)->ip_hlv = sizeof( NGiphdr)>>2;
        /* send message to IP level */
#ifdef NG_DEBUG
{
    char *p;
    p = dgbuf;
    p += ngSPrintf( p, "%I:%d > %I:%d ",
        ti->ti_src, ngNTOHS( ti->ti_sport), ti->ti_dst, ngNTOHS( ti->ti_dport));
    if( ti->ti_flags == 0) p += ngSPrintf( p, ".");
    if( ti->ti_flags & TH_FIN) p += ngSPrintf( p, "F");
    if( ti->ti_flags & TH_SYN) p += ngSPrintf( p, "S");
    if( ti->ti_flags & TH_RST) p += ngSPrintf( p, "R");
    if( ti->ti_flags & TH_PUSH) p += ngSPrintf( p, "P");
    if( ti->ti_flags & TH_URG) p += ngSPrintf( p, "U");
    p += ngSPrintf( p, " %lu:%lu (%d) ", ngNTOHL( ti->ti_seq), ngNTOHL( ti->ti_seq) +
          ngNTOHS( ti->ti_len)-(ti->ti_x2off>>2), ngNTOHS( ti->ti_len)-(ti->ti_x2off>>2));
    if( ti->ti_flags & TH_ACK) p += ngSPrintf( p, "ack %lu ", ngNTOHL( ti->ti_ack));
    ngSPrintf( p, "win %d", ngNTOHS( ti->ti_win));
    ngDebug( NG_DBG_TCP, NG_DBG_OUTPUT, 0, "%s *", dgbuf);
}
#endif
#ifdef NG_IPSTATS_SUPPORTED
        ngTcpStat.tcps_opackets++;
        ngTcpStat.tcps_oresets++;
#endif
        ngIpOutput( bufp, 0);

        return;
    }
    /* free buffer */
    ngBufFree( bufp);
    return;

}

/*****************************************************************************
 * tcpUser()
 *****************************************************************************
 * Process user requests
 *****************************************************************************
 */

static int tcpUser( NGsock *so, int code, void *data, int len, NGsockaddr *addr)
{
    int err;
    NGbuf *bufp, *lastbufp;
    int i, totlen, sendnb, l;
    NGubyte *bufbase, *ciobase;
    int buflen, ciolen;
    NGtcpcb *tp;

    err = NG_EOK;
    bufbase = NULL;

    tp = (NGtcpcb *) so->so_data;
    if( (tp == NULL) && (code != NG_PRU_ATTACH) && (code != NG_PRU_DETACH))
        return( NG_EINVAL);

    switch( code) {

    /*
     * ATTACH
     */
    case NG_PRU_ATTACH:

        /* get a tcp control block */
        NG_QUEUE_OUT( &ngTcp_TcpcbFreeq, tp);
        if( tp == NULL) {
            /* no tcp cb free */
            err = NG_ENOBUFS;
            break;
        }
        /* add socket to the list */
        so->so_node.next = so->so_node.prev = &so->so_node;
        NG_NODE_IN( &ngTcp_Sockq, so);

        /* default values */
        if( (so->so_rcv_hiwat == 0) || (so->so_snd_hiwat == 0)) {
            so->so_rcv_hiwat = NG_TCP_BUF_RECV;
            so->so_rcv_lowat = NG_SOCK_RECV_LOWAT;
            NG_QUEUE_INIT( &so->so_rcv_q, (NG_TCP_BUF_RECV/NG_TCP_MSS));
            so->so_snd_hiwat = NG_TCP_BUF_SEND;
            so->so_snd_lowat = NG_SOCK_SEND_LOWAT;
        }
        /* TCP Control Block */
        so->so_data = tp;
        ngMemSet( tp, 0, sizeof( NGtcpcb));
        tp->tcb_sock = so;
        tp->tcb_state = TCPS_CLOSED;
        tp->tcb_flags = 0;
        tp->tcb_maxseg = NG_TCP_MSS ; /* Why, oh why was this zero?
                                          * [ni@ti.com 2003-10-17] */
        /* reassembly queue */
        tp->tcb_reassq.next = &tp->tcb_reassq;
        tp->tcb_reassq.prev = &tp->tcb_reassq;

        tp->tcb_srtt = NG_TCP_SRTT_BASE;
        tp->tcb_rttvar = NG_TCP_SRTT_DFLT<<2;
        tp->tcb_rttmin = NG_TCP_REXMT_MIN;
        /*lint -e572 (Warning -- Excessive shift value) */
        NG_TCPT_RANGESET( tp->tcb_rxtcur,
            ((NG_TCP_SRTT_BASE>>2) + (NG_TCP_SRTT_DFLT<<2))>>1,
            NG_TCP_REXMT_MIN, NG_TCP_REXMT_MAX);
        /*lint +e572 (Warning -- Excessive shift value) */
        tp->tcb_snd_cwnd = TCP_MAXWIN;
        tp->tcb_snd_ssthresh = TCP_MAXWIN;
        break;

    /*
     * DETACH/DISCONNECT
     * disconnect the socket
     */
    case NG_PRU_DETACH:
    case NG_PRU_DISCONNECT:
        if( tp) {
          if( tp->tcb_state < TCPS_ESTABLISHED) {
            /* not connected, close now */
            tp = ngTcpClose( tp);
          }
          else if( (tp->tcb_sock->so_options & NG_SO_LINGER) &&
                    (tp->tcb_sock->so_linger == 0)) {
            /* drop connection */
            tp = ngTcpDrop( tp, 0);
          }
          else {
            /* start disconnection */
            ngSoIsDisconnecting( tp->tcb_sock);
            /* discard received pending data */
            NG_SOCK_RCVBUF_FLUSH( tp->tcb_sock);
            tp = tcp_usrclosed( tp);
            if( tp) ngTcpOutput( tp);
          }
          if( (code == NG_PRU_DETACH) && tp) {
            /* dont free socket now */
            so->so_state |= NG_SS_NOFDREF;
          }
        }
        break;

    /*
     * ABORT
     * Abort the connection
     */
    case NG_PRU_ABORT:
        ngTcpDrop( tp, NG_ECONNABORTED);
        break;

    /*
     * SHUTDOWN
     * Stop sending data
     * use tcp half-close feature
     */
    case NG_PRU_SHUTDOWN:
        ngSoCantSendMore( so);
        tp = tcp_usrclosed( tp);
        if( tp) err = ngTcpOutput( tp);
        break;

    /*
     * BIND
     * Give an address to the socket
     */
    case NG_PRU_BIND:
        err = ngSoBind( so, addr, (NGsock *) &ngTcp_Sockq, &tcpPort);
        break;

    /*
     * CONNECT
     * Initiate a connection
     */
    case NG_PRU_CONNECT:
        if(tp == NULL)
        {
          return( NG_EINVAL);
        }
        if( so->so_state & NG_SS_ISCONNECTED) {
            err = NG_EISCONN;
            break;
        }
        if( so->so_state & NG_SS_ISCONNECTING) {
            err = NG_EALREADY;
            break;
        }
        /* choose an ephemeral port if necessary */
        if( so->so_lport == 0) {
            err = ngSoBind( so, NULL, (NGsock *) &ngTcp_Sockq, &tcpPort);
            if( err) break;
        }
        /* connect socket */
        err = ngSoConnect( so, addr, (NGsock *) &ngTcp_Sockq, &tcpPort);
        if( err) break;
        /* tcp/ip header template */
        tcp_template( tp);
        /* update socket state */
#ifdef NG_IPSTATS_SUPPORTED
        ngTcpStat.tcps_connattempts++;
#endif
        ngSoIsConnecting( so);
        tp->tcb_state = TCPS_SYN_SENT;
        tp->tcb_tm_keep = NG_TCP_KEEP_INIT;
        /* set sequence numbers */
        tp->tcb_iss = ngTcp_Iss;
        ngTcp_Iss += NG_TCP_ISS_INCR;
        tp->tcb_snd_una = tp->tcb_snd_nxt = tp->tcb_snd_max =
            tp->tcb_snd_up = tp->tcb_iss;

        /* find interface and adjust mss... */
        /* ... if not already set - 22/02/2001 */
        if( tp->tcb_maxseg == 0) {
          NGifnet *netp;
          NGuint gaddr;

          netp = ngIpRouteLookup( tp->tcb_sock->so_faddr, &gaddr,
                    so->so_options & NG_IPOUTPUT_DONTROUTE);
          /* if destination host is directly attached to the interface */
          /* take interface mtu as mss */
          if( netp && (tp->tcb_sock->so_faddr == gaddr))
            tp->tcb_maxseg = netp->if_mtu - sizeof( NGtcpiphdr);
          else
            tp->tcb_maxseg = NG_TCP_MSS;
        }

        /* cannot have maxseg > bufdatamax */
        if( tp->tcb_maxseg > (int) (ngBufDataMax - sizeof( NGtcpiphdr)))
          tp->tcb_maxseg = ngBufDataMax - sizeof( NGtcpiphdr);

        /* send initial SYN */
        err = ngTcpOutput( tp);

        break;

    /*
     * LISTEN
     * Prepare the socket to accept connections
     */
    case NG_PRU_LISTEN:
        if(tp == NULL)
        {
          return( NG_EINVAL);
        }
        /* choose an ephemeral port if necessary */
        if( so->so_lport == 0) {
            err = ngSoBind( so, NULL, (NGsock *) &ngTcp_Sockq, &tcpPort);
            if( err) break;
        }
        tp->tcb_state = TCPS_LISTEN;
        break;

    /*
     * RCVD
     * Eventually send window update segment
     */
    case NG_PRU_RCVD:
        ngTcpOutput( tp);
        break;

    /*
     * SEND
     * SENDOOB
     * Copy data to send buffer
     */
    case NG_PRU_SEND:
    case NG_PRU_SENDOOB:
        if(tp == NULL)
        {
          return( NG_EINVAL);
        }
        /* number of bytes to transfert */
        totlen = 0;
        for( i=0; i<len; i++) totlen += ((NGiovec *) data)[i].iov_len;

        /* data waiting ? */
        bufp = tp->tcb_buf_waitack;
        if( bufp) {
            /* find last buffer */
            while( bufp->buf_next) bufp = bufp->buf_next;
/***************************/
    if( bufp->buf_flags & NG_BUFF_DONTFREE) {
        /* buffer already sent, allocate new buffer */
        ngBufAlloc( bufp->buf_next);
        if( bufp->buf_next == NULL) {
            err = NG_ENOBUFS;
            break;
        }
        bufp = bufp->buf_next;
        bufp->buf_iovcnt = 0;
    }
/***************************/
            bufbase = ((NGubyte *) bufp) + ngTcpBufDataOffset + bufp->buf_iovcnt;
            buflen = tp->tcb_maxseg - bufp->buf_iovcnt;
        }
        else {
            tp->tcb_buf_ackcc = 0;
            buflen = 0;
        }

        /* source buffer */
        i = 0;
        ciobase = ((NGiovec *)data)[0].iov_base;
        ciolen = ((NGiovec *)data)[0].iov_len;
        sendnb = totlen;
        while( totlen > 0) {
            if( buflen == 0) {
                lastbufp = bufp;
                /* alloc new buffer */
                ngBufAlloc( bufp);
                if( bufp == NULL) {
                    err = NG_ENOBUFS;
                    break;
                }
                /* buffer link */
                if( lastbufp) lastbufp->buf_next = bufp;
                else tp->tcb_buf_waitack = bufp;
                /* adjust pointers */
                bufbase = ((NGubyte *) bufp) + ngTcpBufDataOffset;
                buflen = tp->tcb_maxseg;
                bufp->buf_iovcnt = 0;
                bufp->buf_next = NULL;
            }

            l = buflen > ciolen ? ciolen : buflen;
            if( bufbase != NULL ) {
                ngMemCpy( bufbase, ciobase, l);
            }	 
	    else {
                err = NG_EINVAL;
	        break ;
	    }
            
            
            ciolen -= l;
            if( ciolen == 0) {
                i++; /* next user buffer */
                ciobase = ((NGiovec *)data)[i].iov_base;
                ciolen = ((NGiovec *)data)[i].iov_len;
            }
            else ciobase += l;
            buflen -= l;
            bufbase += l;
            if( bufp != NULL) {	    
                bufp->buf_iovcnt += l;
	    }
	    else {
                err = NG_ENOBUFS;
	        break; 
            }
	               
            so->so_snd_cc += l;
            totlen -= l;
        }

        if( !err) {
            if( code == NG_PRU_SENDOOB) {
              /* set uregnt pointer */
              tp->tcb_snd_up = tp->tcb_snd_una + so->so_snd_cc;
              /* force transmission of segment */
              tp->tcb_force = 1;
              ngTcpOutput( tp);
              tp->tcb_force = 0;
            }
            else {
              ngTcpOutput( tp);
            }
            err = sendnb;
        }
        break;

    /*
     * RCVOOB
     * Read out-of-band data
     */
    case NG_PRU_RCVOOB:
      if(tp == NULL)
      {
        return( NG_EINVAL);
      }
      /* read urgent byte if any */
      if( ((so->so_oobmark == 0) && ((so->so_state & NG_SS_RCVATMARK) == 0))
          || (so->so_options & NG_SO_OOBINLINE) ||
             (tp->tcb_oobflags & NG_TCPOOB_HADDATA) ) {
        err = NG_EINVAL;
        break;
      }
      if( !(tp->tcb_oobflags & NG_TCPOOB_HAVEDATA)) {
        err = NG_EWOULDBLOCK;
        break;
      }
      *((NGubyte *) data) = tp->tcb_iobc;
      if( !(len & NG_IO_PEEK)) {
        /* clear HAVEDATA, set HADDATA */
        tp->tcb_oobflags &= ~NG_TCPOOB_HAVEDATA;
        tp->tcb_oobflags |= NG_TCPOOB_HADDATA;
      }
      break;

    case NG_PRU_SENDBUF:
    default:
        err = NG_EOPNOTSUPP;
    }

    return( err);
}

/*****************************************************************************
 * tcpUserOption()
 *****************************************************************************
 * Process user option requests
 *****************************************************************************
 */

static int tcpUserOption( NGsock *so, int level, int optname,
                          void *optarg, int *optlen)
{
    if( (level != NG_IOCTL_TCP) || !so->so_data) return( NG_EOPNOTSUPP);
    switch( optname) {
    case NG_SO_SET|NG_TCP_NODELAY:
        if( *optlen != sizeof( int)) return( NG_EINVAL);
        if( *((int *) optarg)) {
            ((NGtcpcb *) so->so_data)->tcb_flags |= NG_TF_NODELAY;
        }
        else {
            ((NGtcpcb *) so->so_data)->tcb_flags &= ~NG_TF_NODELAY;
        }
        break;
    case NG_SO_SET|NG_TCP_MAXSEG:
        if( (*optlen != sizeof( int)) || (*((int *) optarg) <= 0) ||
            ( (((NGtcpcb *) so->so_data)->tcb_state > NG_TCPS_LISTEN) &&
              (*((int *) optarg) > ((NGtcpcb *) so->so_data)->tcb_maxseg) ) )
            return( NG_EINVAL);
        ((NGtcpcb *) so->so_data)->tcb_maxseg = *((int *) optarg);
        break;
    case NG_SO_GET|NG_TCP_NODELAY:
    case NG_SO_GET|NG_TCP_MAXSEG:
        if( *optlen < (signed)sizeof( int)) return( NG_EINVAL);
        switch( optname) {
        case NG_SO_GET|NG_TCP_NODELAY:
            *((int *) optarg) = ((NGtcpcb *) so->so_data)->tcb_flags & NG_TF_NODELAY;
            break;
        case NG_SO_GET|NG_TCP_MAXSEG:
            *((int *) optarg) = ((NGtcpcb *) so->so_data)->tcb_maxseg;
            break;
        }
        *optlen = sizeof( int);
        break;
    default:
        return( NG_ENOPROTOOPT);
    }
    return( NG_EOK);
}

/*****************************************************************************
 * ngProto_TCP()
 *****************************************************************************
 * Protocol structure definition
 *****************************************************************************
 */

const NGproto ngProto_TCP = {
    "TCP",
    NG_SOCK_STREAM,
    NG_PR_CONNREQUIRED|NG_PR_WANTRCVD,
    NG_PROTO_TCP,
    tcpInit,
    tcpCntl,
    tcpTimer,
    tcpInput,
    (int(*)())tcpUserOption,
    NULL,
    (int(*)())tcpUser
};

