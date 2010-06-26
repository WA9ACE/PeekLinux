/*****************************************************************************
 * $Id: tcp_subr.c,v 1.4 2002/04/23 10:20:04 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Transmission Control Protocol - Internal Routines
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
 * ngTcpDrop()
 * ngTcpClose()
 * ngTcpSetPersist()
 * ngTcpXmitTimer()
 * ngTcpReass()
 *----------------------------------------------------------------------------
 * 21/12/98 - Regis Feneon
 * 22/12/98 -
 *  Added ngTcpReass() function
 * 07/04/99 -
 *  ngTcpClose() now call ngSoIsDisconnected()
 * 13/09/99 -
 *  ngTcpReass()
 *  now sequence number stored in (NGuint *) buf_iov (after NGbuf structure)
 *  now tcp flags stored in buf_iovcnt
 *  function arguments changed
 *  bug in overlapping data test for succeeding segments
 * 17/11/2000 -
 *  added statistics
 * 19/04/2002 -
 *  ngTcpDrop(): errno argument renamed err
 *****************************************************************************/

#include <ngip.h>
#include <ngtcp.h>
#include <ngip/ip.h>
#include <ngip/tcp.h>
#include <ngdsock.h>

/* retransmission delays */
const int ngTcp_Backoff[] =
    { 1, 2, 4, 8, 16, 32, 64, 64, 64, 64, 64, 64, 64 };

/*****************************************************************************
 * ngTcpDrop()
 *****************************************************************************
 * Drop the connection by sending RST,
 * report error to process
 *****************************************************************************
 * Parameters:
 *  tp      tcp control block
 *  err     error to report to process
 */

NGtcpcb *ngTcpDrop( NGtcpcb *tp, int err)
{
    if( tp->tcb_state >= TCPS_SYN_RECEIVED) {
        tp->tcb_state = TCPS_CLOSED;
        ngTcpOutput( tp);
#ifdef NG_IPSTATS_SUPPORTED
        ngTcpStat.tcps_drops++;
#endif
    }
#ifdef NG_IPSTATS_SUPPORTED
    else {
        ngTcpStat.tcps_conndrops++;
    }
#endif
    if( (err == NG_ETIMEDOUT) && tp->tcb_softerror)
        err = tp->tcb_softerror;
    tp->tcb_sock->so_error = err;
    return( ngTcpClose( tp));
}

/*****************************************************************************
 * ngTcpClose()
 *****************************************************************************
 * Close the socket
 * free all resources associated with the socket
 *****************************************************************************
 * Parameters:
 *  tp      tcp control block
 */

NGtcpcb *ngTcpClose( NGtcpcb *tp)
{
    NGsock *so;
    NGbuf *bufp;

    /* free the reassembly queue */
    while( tp->tcb_reassq.next != &tp->tcb_reassq) {
        bufp = (NGbuf *) tp->tcb_reassq.next;
        NG_NODE_DETACH( tp->tcb_reassq.next);
        ngBufFree( bufp);
    }

    /* free send buffers */
    NG_TCPBUF_FREE( tp);

    /* save socket pointer */
    so = tp->tcb_sock;

    /* free the control block */
    NG_QUEUE_IN( &ngTcp_TcpcbFreeq, tp);
    so->so_data = NULL;

    /* detach socket */
    if( so == ngTcp_LastSock) ngTcp_LastSock = NULL;
    NG_NODE_DETACH( so);

    /* socket is disconnected */
    ngSoIsDisconnected( so);

    /* free socket if not used by application */
    if( so->so_state & NG_SS_NOFDREF) ngSoFree( so);

    return( NULL);
}

/*****************************************************************************
 * ngTcpSetPersist()
 *****************************************************************************
 * Set Persist timer
 *****************************************************************************
 * Parameters:
 *  tp      tcp control block
 */

void ngTcpSetPersist( NGtcpcb *tp)
{
    int t;

    t = ((tp->tcb_srtt >> 2) + tp->tcb_rttvar) >> 1;

    NG_TCPT_RANGESET( tp->tcb_tm_persist,
        t * ngTcp_Backoff[tp->tcb_rxtshift],
        NG_TCP_PERS_MIN, NG_TCP_PERS_MAX);
    if( tp->tcb_rxtshift < NG_TCP_REXMT_MAXSHIFT)
        tp->tcb_rxtshift++;
}

/*****************************************************************************
 * ngTcpXmitTimer()
 *****************************************************************************
 * Update the smoothed RTT estimator (srtt)
 * and the smoothed mean deviation estimator (rttvar)
 *****************************************************************************
 * Parameters:
 *  tp      tcp control block
 *  rtt     rtt measurement
 */

void ngTcpXmitTimer( NGtcpcb *tp, int rtt)
{
    int delta;

    if( tp->tcb_srtt) {
        /*
         * rtt stored in fixed point format 13.3
         * srtt = rtt/8 =srtt*7/8
         */
        delta = rtt - 1 - (tp->tcb_srtt >> NG_TCP_RTT_SHIFT);
        if( (tp->tcb_srtt += delta) <= 0)
            tp->tcb_srtt = 1;

        if( delta < 0)
            delta = -delta;

        delta -= (tp->tcb_rttvar >> NG_TCP_RTTVAR_SHIFT);
        if( (tp->tcb_rttvar += delta) <= 0)
            tp->tcb_rttvar = 1;
    }
    else {
        /*
         * no rtt mesured yet
         */
        tp->tcb_srtt = rtt << NG_TCP_RTT_SHIFT;
        tp->tcb_rttvar = rtt << (NG_TCP_RTTVAR_SHIFT - 1);
    }

    tp->tcb_rtt = 0;
    tp->tcb_rxtshift = 0;

    NG_TCPT_RANGESET( tp->tcb_rxtcur, NG_TCP_REXMTVAL( tp),
                tp->tcb_rttmin, NG_TCP_REXMT_MAX);

    tp->tcb_softerror = 0;
}

/*****************************************************************************
 * ngTcpReass()
 *****************************************************************************
 * Reassembly of received segments
 *****************************************************************************
 * Parameters:
 *  tp          tcp control block
 *  bufseq      sequence number of received segment data
 *  tcpflags    tcp flags of received segment
 *  bufp        message buffer containing segment data
 * Return value: tcp control block new flags
 */

int ngTcpReass( NGtcpcb *tp, NGuint bufseq, int tcpflags, NGbuf *bufp)
{
    NGbuf *b, *tbufp;
    NGsock *so;
    int i, flags;

    so = tp->tcb_sock;

    if( bufp != NULL) {

        /* find segment preceding the new one */
        for( b = (NGbuf *) tp->tcb_reassq.next;
          b != (NGbuf *) &tp->tcb_reassq; b = (NGbuf *) b->buf_node.next) {
             if( NG_SEQ_GT( *((NGuint *) b->buf_iov), bufseq))
                break;
        }
        /*
         * drop overlapped data from preceding segment
         */
        if( b->buf_node.prev != &tp->tcb_reassq) {
            i = *((NGuint *) (((NGbuf *) b->buf_node.prev)->buf_iov)) - bufseq
                + b->buf_datalen;
            if( i > 0) {
                if( i >= bufp->buf_datalen) {
                    /* received data is totally duplicated */
                    ngBufFree( bufp);
                    return( 0);
                }
                /* remove i bytes from the beginning of the segment */
                bufp->buf_datap += i;
                bufp->buf_datalen -= i;
                bufseq += i;
            }

        }
        /*
         * drop overlapped data from next segments
         */
        while( b != (NGbuf *) &tp->tcb_reassq) {
            i = (bufseq + bufp->buf_datalen) - *((NGuint *) b->buf_iov); /* 13/09/99 */
            if( i < 0) break;
            if( i < b->buf_datalen) {
                b->buf_datap += i;
                b->buf_datalen -= i;
                *((NGuint *) b->buf_iov) += i;
                break;
            }
            /* segment is totally overlapped, free it */
            tbufp = b;
            b = (NGbuf *) b->buf_node.next;
            NG_NODE_DETACH( &tbufp->buf_node);
            ngBufFree( tbufp);
        }
        /* add new segment in the list */
        bufp->buf_iov = (NGiovec *) (((NGubyte *) bufp) + sizeof( NGbuf));
        *((NGuint *) bufp->buf_iov) = bufseq; /* save sequence number */
        bufp->buf_iovcnt = tcpflags; /* save TH_FIN flag */
        bufp->buf_node.next = &b->buf_node;
        bufp->buf_node.prev = b->buf_node.prev;
        b->buf_node.prev->next = &bufp->buf_node;
        b->buf_node.prev = &bufp->buf_node;
    }

    /*
     * move data to user receive buffer
     */
    if( tp->tcb_state < TCPS_SYN_RECEIVED) return( 0);
    b = (NGbuf *) tp->tcb_reassq.next;
    if( (b == (NGbuf *) &tp->tcb_reassq) ||
        (*((NGuint *) b->buf_iov) != tp->tcb_rcv_nxt))
        return( 0);
    if( (tp->tcb_state == TCPS_SYN_RECEIVED) && b->buf_datalen)
        return( 0);

    do {
        tp->tcb_rcv_nxt += b->buf_datalen;
        flags = b->buf_iovcnt & TH_FIN;
        tbufp = (NGbuf *) b->buf_node.next;
        NG_NODE_DETACH( &b->buf_node);
        if( so->so_state & NG_SS_CANTRCVMORE) {
            ngBufFree( b);
        }
        else {
            /* append data to receive buffer */
            b->buf_iov = NULL;
            b->buf_iovcnt = 0;
            NG_QUEUE_IN( &so->so_rcv_q, b);
            so->so_rcv_cc += b->buf_datalen;
        }
        b = tbufp;
    } while( (b != (NGbuf *) &tp->tcb_reassq) &&
             (*((NGuint *) b->buf_iov) == tp->tcb_rcv_nxt));

    /* wake up reading process */
    ngSoDataReceived( so);

    return( flags);
}

