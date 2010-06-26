/*****************************************************************************
 * $Id: tcp_out.c,v 1.7.2.1 2002/10/28 18:49:49 rf Exp $
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
 * ngTcpOutput()
 *----------------------------------------------------------------------------
 * 17/12/98 - Regis Feneon
 * 11/04/99 -
 *  bug in calculation of data offset in buffer list (!)
 * 19/10/99 -
 *  bad test (or bad taste) > instead of >=
 * 18/02/2000 - Adrien Felon
 *  ngTcpOutput() - adding argument "flag" for ngIpOutput() call
 * 25/02/2000 - Regis Feneon
 *  added PUSH flag when all data has been transmitted from send buffer
 * 20/03/2000 -
 *  set len to 0 when no data buffer to send
 *  copy data to new buffer when buffer is not totaly sent in a segment
 * 21/03/2000 -
 *  added (NGint) cast in tests for receive window
 * 17/11/2000 -
 *  added statistics
 * 23/01/2001 -
 *  corrections in NGiphdr, NGtcphdr & NGtcpiphdr structures
 * 18/02/2001 -
 *  added DONTROUTE option
 * 17/04/2001 -
 *  do not reemit segments when transmission is still in progress
 * 28/10/2002 -
 *  removed some warnings with msvc
 *****************************************************************************/

#include <ngip.h>
#include <ngtcp.h>
#include <ngip/ip.h>
#include <ngip/tcp.h>

static const int tcp_outflags[] = {
    TH_RST|TH_ACK,
    0,
    TH_SYN,
    TH_SYN|TH_ACK,
    TH_ACK,
    TH_ACK,
    TH_FIN|TH_ACK,
    TH_FIN|TH_ACK,
    TH_FIN|TH_ACK,
    TH_ACK,
    TH_ACK
};

/*****************************************************************************
 * ngTcpOutput()
 *****************************************************************************
 * TCP output function
 *****************************************************************************
 * Parameter:
 *  so      socket descriptor
 * Return value: 0 or error code
 */

int ngTcpOutput( NGtcpcb *tp)
{
    int off, idle, flags, error, ictl;
    NGint win;
    int len, reallen, optlen;
    int sendalot, bufoff;
    NGbuf *bufp, *tbufp;
    NGtcpiphdr *ti;
#ifdef NG_DEBUG
    char dgbuf[128];
#endif
    reallen = 0;
    bufoff = 0;
    bufp = NULL;

    /* slow start: set congestion window to one segment */
    idle = (tp->tcb_snd_max == tp->tcb_snd_una);
    if( idle && (tp->tcb_idle >= tp->tcb_rxtcur)) {
        tp->tcb_snd_cwnd = tp->tcb_maxseg;
    }

again:
    sendalot = 0;

    /* first data byte to send */
    off = tp->tcb_snd_nxt - tp->tcb_snd_una;

    win = NG_MIN( tp->tcb_snd_wnd, tp->tcb_snd_cwnd);
    flags = tcp_outflags[tp->tcb_state];

    if( tp->tcb_force) {
        if( win == 0) {
            if( off < tp->tcb_sock->so_snd_cc) /* buf_cc */
                flags &= ~TH_FIN;
            win = 1;
        }
        else {
            tp->tcb_tm_persist = 0;
            tp->tcb_rxtshift = 0;
        }
    }
    len = NG_MIN( tp->tcb_sock->so_snd_cc, win) - off; /* buf_cc */
    if( len < 0) {
        len = 0;
        if( win == 0) {
            tp->tcb_tm_rexmt = 0;
            tp->tcb_snd_nxt = tp->tcb_snd_una;
        }
    }

    /* pointer on the buffer containing the start of the data to send */
    /* total offset = buf_ackcc + off */
    if( len) { /* 21/03/2000 */
        bufp = tp->tcb_buf_waitack;
        bufoff = tp->tcb_buf_ackcc + off;
        while( bufp) {
            if( bufoff >= bufp->buf_iovcnt) { /* >= 19/10/99 */
                bufoff -= bufp->buf_iovcnt;
                bufp = bufp->buf_next;
            }
            else break;
        }

        reallen = len;
        if( bufp) {
            if( len > (bufp->buf_iovcnt - bufoff)) {
                len = bufp->buf_iovcnt - bufoff;
                sendalot = 1;
            }
        }
        else len = 0; /* 20/03/2000 */
    }

    if( NG_SEQ_LT( tp->tcb_snd_nxt + len, tp->tcb_snd_una
                     + tp->tcb_sock->so_snd_cc)) /* buf_cc */
        flags &= ~TH_FIN;

    win = tp->tcb_sock->so_rcv_hiwat - tp->tcb_sock->so_rcv_cc;

    /*
     * silly window avoidance
     */
    if( len) {
        if( reallen >= tp->tcb_maxseg)
            goto send;
        if( (idle || (tp->tcb_flags & NG_TF_NODELAY)) &&
            (reallen + off >= tp->tcb_sock->so_snd_cc)) /* buf_cc */
            goto send;
        if( tp->tcb_force)
            goto send;
        if( reallen >= (int) (tp->tcb_max_sndwnd>>1))
            goto send;
        if( NG_SEQ_LT( tp->tcb_snd_nxt, tp->tcb_snd_max))
            goto send;
    }

    if( win > 0) {
        NGint adv = win - ((NGint) (tp->tcb_rcv_adv - tp->tcb_rcv_nxt));

        if( adv >= (tp->tcb_maxseg<<1))
            goto send;
        if( (adv<<1) >= tp->tcb_sock->so_rcv_hiwat)
            goto send;
    }

    /*
     * send if we owe peer an ACK
     */
    if( tp->tcb_flags & NG_TF_ACKNOW)
        goto send;
    if( flags & (TH_SYN|TH_RST))
        goto send;
    if( NG_SEQ_GT( tp->tcb_snd_up, tp->tcb_snd_una))
        goto send;

    /*
     * send if we need to send the FIN
     */
    if( (flags & TH_FIN) && ( ((tp->tcb_flags & NG_TF_SENTFIN) == 0) ||
     (tp->tcb_snd_nxt == tp->tcb_snd_una)) )
        goto send;

    if( tp->tcb_sock->so_snd_cc && (tp->tcb_tm_rexmt == 0) && /* buf_cc */
        tp->tcb_tm_persist == 0) {
        tp->tcb_rxtshift = 0;
        /* set persist timer tcp_setpersist() */
        ngTcpSetPersist( tp);

        tp->tcb_tm_persist = (((tp->tcb_srtt >> 2) + tp->tcb_rttvar) >> 1)
                              * ngTcp_Backoff[0];
        if( tp->tcb_tm_persist < NG_TCP_PERS_MIN)
            tp->tcb_tm_persist = NG_TCP_PERS_MIN;
        if( tp->tcb_tm_persist > NG_TCP_PERS_MAX)
            tp->tcb_tm_persist = NG_TCP_PERS_MAX;

    }
    /* nothing to send... */
    return( 0);

send:
    /* NO OPTION IN DATA SEGMENTS */
    optlen = 0;
    if( len && (bufp != NULL) ) {

        /* is data buffer busy ? */
        ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE);
        if( bufp->buf_flags & NG_BUFF_BUSY) {
            ngOSIntrCtl( ictl);
            error = NG_ENOBUFS;
            /* retry later... no! 18/04/2001 */
            /* do not reemit when transmission is still in progress */
            /* tp->tcb_flags |= NG_TF_DELACK; */
            goto out;
        }
        ngOSIntrCtl( ictl);

        bufp->buf_datap = ((NGubyte *) bufp) + ngTcpBufDataOffset + bufoff;
        /* 20/03/2000 - copy to new buffer if buffer not totaly sent */
        if( len != bufp->buf_iovcnt) {
            /* alloc buffer */
            ngBufAlloc( tbufp);
            if( tbufp == NULL) {
                error = NG_ENOBUFS;
                /* retry later */
                tp->tcb_flags |= NG_TF_DELACK;
                goto out;
            }
            tbufp->buf_datap = ((NGubyte *) tbufp) + ngTcpBufDataOffset;
            /* copy data to new buffer */
            ngMemCpy( tbufp->buf_datap, bufp->buf_datap, len);
            bufp = tbufp;
        }
        else {
            /* use socket data buffer */
            /* keep buffer after sending for retransmissions */
            bufp->buf_flags |= (NG_BUFF_DONTFREE|NG_BUFF_BUSY);
        }
        bufp->buf_datalen = len;

#ifdef NG_IPSTATS_SUPPORTED
        /* retransmitted data ? (dont count window probes) */
        if( NG_SEQ_LT( tp->tcb_snd_nxt, tp->tcb_snd_max) &&
            !(tp->tcb_force && (len == 1)) ) {
          ngTcpStat.tcps_orexmts++;
        }
#endif

        /* is all data transmitted ? PUSH! (25/02/2000) */
        if( (off + len) == tp->tcb_sock->so_snd_cc) {
            flags |= TH_PUSH;
        }

    }
    else {
        /* allocate a single buffer */
        ngBufAlloc( bufp);
        if( bufp == NULL) {
            error = NG_ENOBUFS;
            goto out;
        }
        bufp->buf_datap = ((NGubyte *) bufp) + ngTcpBufDataOffset;

        if( flags & TH_SYN) {
            /* if SYN flag set MSS option */
            bufp->buf_datap[0] = TCPOPT_MAXSEG;
            bufp->buf_datap[1] = TCPOPTLEN_MAXSEG;
            *((NGushort *)(bufp->buf_datap+2)) = ngHTONS( (NGushort) tp->tcb_maxseg);
            optlen = TCPOPTLEN_MAXSEG;
        }
        bufp->buf_datalen = optlen;
    }

    /* fill in IP + TCP headers */
    bufp->buf_datap -= sizeof( NGtcpiphdr);
    bufp->buf_datalen += sizeof( NGtcpiphdr);

    ti = (NGtcpiphdr *) bufp->buf_datap;
    /* copy header template */
    ngMemCpy( ti, &tp->tcb_thdr, sizeof( NGtcpiphdr));

    if( (flags & TH_FIN) && (tp->tcb_flags & NG_TF_SENTFIN) &&
        (tp->tcb_snd_nxt == tp->tcb_snd_max))
        tp->tcb_snd_nxt--;

    if( len || (flags & (TH_SYN|TH_FIN)) || tp->tcb_tm_persist)
        ti->ti_seq = ngHTONL( tp->tcb_snd_nxt);
    else
        ti->ti_seq = ngHTONL( tp->tcb_snd_max);

    ti->ti_ack = ngHTONL( tp->tcb_rcv_nxt);

    if( optlen) {
        ti->ti_x2off = (sizeof( NGtcphdr) + optlen)<<(NG_TCPOFF_SHIFT-2);
    }
    ti->ti_flags = flags;

    /*
     * calculate receive window
     */
    if( (win < (NGint) (tp->tcb_sock->so_rcv_hiwat>>2)) &&
        (win < (NGint) tp->tcb_maxseg) )
        win = 0;
    if( win > (NGint) TCP_MAXWIN)
        win = TCP_MAXWIN;
    if( win < (NGint) (tp->tcb_rcv_adv - tp->tcb_rcv_nxt))
        win = tp->tcb_rcv_adv - tp->tcb_rcv_nxt;

    ti->ti_win = ngHTONS( (NGushort) win);

    if( NG_SEQ_GT( tp->tcb_snd_up, tp->tcb_snd_nxt)) {
        ti->ti_urp = ngHTONS( (NGushort) (tp->tcb_snd_up - tp->tcb_snd_nxt));
        ti->ti_flags |= TH_URG;
    }
    else {
        tp->tcb_snd_up = tp->tcb_snd_una;
    }

    if( len + optlen)
        ti->ti_len = ngHTONS( (NGushort) (sizeof( NGtcphdr) + optlen + len));

    ti->ti_sum = ngIpCksum( bufp->buf_datap, sizeof( NGtcpiphdr) + optlen + len,
                           NULL, 0);

    /*
     * In transmit state, arrange for retransmit.
     * In persist state, just set snd_max
     */
    if( (tp->tcb_force == 0) || (tp->tcb_tm_persist == 0)) {
        NGuint startseq = tp->tcb_snd_nxt;

        if( flags & (TH_SYN|TH_FIN)) {
            if( flags & TH_SYN)
                tp->tcb_snd_nxt++;
            if( flags & TH_FIN) {
                tp->tcb_snd_nxt++;
                tp->tcb_flags |= NG_TF_SENTFIN;
            }
        }
        tp->tcb_snd_nxt += len;
        if( NG_SEQ_GT( tp->tcb_snd_nxt, tp->tcb_snd_max)) {
            tp->tcb_snd_max = tp->tcb_snd_nxt;

            if( tp->tcb_rtt == 0) {
                tp->tcb_rtt = 1;
                tp->tcb_rtseq = startseq;
            }
        }

        /* set retransmit timer */
        if( (tp->tcb_tm_rexmt == 0) && (tp->tcb_snd_nxt != tp->tcb_snd_una)) {
            tp->tcb_tm_rexmt = tp->tcb_rxtcur;
            if( tp->tcb_tm_persist) {
                tp->tcb_tm_persist = 0;
                tp->tcb_rxtshift = 0;
            }
        }
    }
    else if( NG_SEQ_GT( tp->tcb_snd_nxt + len, tp->tcb_snd_max))
        tp->tcb_snd_max = tp->tcb_snd_nxt + len;

    /*
     * send to IP level
     */
    ((NGiphdr *) ti)->ip_len = sizeof( NGtcpiphdr) + optlen + len;
    ((NGiphdr *) ti)->ip_hlv = sizeof( NGiphdr)>>2;
    ((NGiphdr *) ti)->ip_ttl = tp->tcb_sock->so_ttl;
    ((NGiphdr *) ti)->ip_tos = tp->tcb_sock->so_tos;

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
    p += ngSPrintf( p, "win %d", ngNTOHS( ti->ti_win));
    if( optlen) {
        ngSPrintf( p, " <mss> %d", tp->tcb_maxseg);
    }
    ngDebug( NG_DBG_TCP, NG_DBG_OUTPUT, 0, "%s", dgbuf);
}
#endif

#ifdef NG_IPSTATS_SUPPORTED
    ngTcpStat.tcps_opackets++;
    if( ti->ti_flags & TH_RST) {
      ngTcpStat.tcps_oresets++;
    }
#endif

    error = ngIpOutput( bufp, tp->tcb_sock->so_options & NG_SO_DONTROUTE);

    if( error) {
out:
        if( error == NG_ENOBUFS) return( 0);

        if( ((error == NG_EHOSTUNREACH) || (error == NG_ENETDOWN))
            && (tp->tcb_state >= TCPS_SYN_RECEIVED)) {
            tp->tcb_softerror = error;
            return( 0);
        }
        return( error);
    }

    /*
     * Data sent...
     */
    if( (win > 0) && NG_SEQ_GT( tp->tcb_rcv_nxt + win, tp->tcb_rcv_adv))
        tp->tcb_rcv_adv = tp->tcb_rcv_nxt + win;
    tp->tcb_flags &= ~(NG_TF_ACKNOW|NG_TF_DELACK);

    if( sendalot) goto again; /* more data to send */

    return( 0);
}

