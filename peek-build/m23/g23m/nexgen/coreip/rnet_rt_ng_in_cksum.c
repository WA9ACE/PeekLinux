/*****************************************************************************
 * $Id: in_cksum.c,v 1.3 2001/11/20 14:36:07 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet checksum computation
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
 * ngIpCksum()
 *----------------------------------------------------------------------------
 * 22/09/98 - Regis Feneon
 * 01/02/99 -
 *  performances enhancements
 *  added suport for iovec structures
 * 25/06/99 -
 *  (u_long) casting was missing in REDUCE macro (!)
 * 05/11/99 -
 *  bug with odd buffer addresses (!!)
 * 25/05/2000 -
 *  simplified ADDCARRY() and return
 * 20/11/2001 -
 *  in_cksum() renamed ngIpCksum()
 *****************************************************************************/

#include <ngip.h>

/*****************************************************************************
 * ngIpCksum()
 *****************************************************************************
 * Standart IP checksum calculation routine
 *****************************************************************************
 * Parameters:
 *  sptr        pointer to buffer
 *  len         size of buffer
 *  iov         externale buffers
 *  iovcnt      number of external buffers
 * Return value: checksum value
 */

#define ADDCARRY(x) { if((x) > 65535UL) (x) -= 65535UL; }
#define REDUCE { \
    l_util.l = sum; \
    sum = (u_long) l_util.s[0] + l_util.s[1]; \
    ADDCARRY( sum); \
}

NGushort ngIpCksum( void *sptr, int len, NGiovec *iov, int iovcnt)
{
    NGushort *w;
    u_long sum;
    int mlen;
    int bswapped, iovi;
    union {
        NGubyte c[2];
        NGushort s;
    } s_util;
    union {
        NGushort s[2];
        NGuint l;
    } l_util;

    w = sptr;
    mlen = len;
    iovi = -1;
    sum = 0;
    bswapped = 0;

    while( iovi < iovcnt) {
        if( iovi >= 0) {
            w = iov[iovi].iov_base;
            if( mlen == -1) {
                /* a word is accross two buffers */
                s_util.c[1] = *((NGubyte *) w);
                sum += s_util.s;
                w = (NGushort *) ((NGubyte *) w + 1);
                mlen = iov[iovi].iov_len - 1;
            }
            else mlen = iov[iovi].iov_len;
        }
        /* force to even boundary */
        if( (((int) w) & 1) && (mlen > 0)) {
            REDUCE;
            sum <<= 8;
            s_util.c[0] = *((NGubyte *) w);
            w = (NGushort *) ((NGubyte *) w + 1);
            mlen--;
            bswapped = 1;
        }

        /* compute checksum */
        while( mlen >= 32) {
            sum += *w++; sum += *w++; sum += *w++; sum += *w++;
            sum += *w++; sum += *w++; sum += *w++; sum += *w++;
            sum += *w++; sum += *w++; sum += *w++; sum += *w++;
            sum += *w++; sum += *w++; sum += *w++; sum += *w++;
            mlen -= 32;
        }
        while( mlen >= 8) {
            sum += *w++; sum += *w++; sum += *w++; sum += *w++;
            mlen -= 8;
        }
        if( mlen || bswapped) {
            REDUCE;
            while( (mlen -= 2) >= 0) {
                sum += *w++;
            }
            if( bswapped) {
                REDUCE;
                sum <<= 8;
                bswapped = 0;
                if( mlen == -1) {
                    s_util.c[1] = *((NGubyte *) w);
                    sum += s_util.s;
                    mlen = 0;
                }
                else mlen = -1; /* 05/11/99 */
            }
            else if( mlen == -1) {
                s_util.c[0] = *((NGubyte *) w);
            }
        }
        /* next buffer */
        iovi++;
    } /* while */
    if( mlen == -1) {
        /* one byte left */
        s_util.c[1] = 0;
        sum += s_util.s;
    }
    REDUCE;
    return (NGushort) ~sum;
}

