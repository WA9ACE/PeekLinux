/*****************************************************************************
 * $Id: iovcpy.c,v 1.3 2001/05/28 17:35:57 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Buffer Management Routines
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
 * ngIovCpy()
 *----------------------------------------------------------------------------
 * 29/05/99 -
 *  iov_copy moved to iovcpy.c (renamed ngIovCpy())
 * 28/05/2001 - Regis Feneon
 *  update dst after ngMemCpy (!)
 *****************************************************************************/

#include <ngos.h>
#include <ngos/ng_stdio.h>

/*****************************************************************************
 * ngIovCpy
 *****************************************************************************
 * copy data from iov structure to linear buffer
 *****************************************************************************
 * dst      destination buffer
 * iov      iovec array
 * iovcnt   iovec array size
 * iov_off  offset in iovec structure to copy from
 * len      number of bytes to copy
 */

void ngIovCpy( NGubyte *dst, NGiovec *iov, int iovcnt, int iov_off, int len)
{
    NGubyte *src;
    int i, i_off;

    /* set source pointer */
    i = 0;
    i_off = 0;
    while( i_off < iov_off) {
        if( (iov_off - i_off) > iov[i].iov_len) {
            i_off += iov[i].iov_len;
            i++;
            if( i > iovcnt) return; /* it should not happen (!) */
            continue;
        }
        i_off = iov_off - i_off;
        break;
    }
    src = ((NGubyte *) iov[i].iov_base) + i_off;
    i_off = iov[i].iov_len - i_off;

    /* copy data */
    while( len > 0) {
        if( len > i_off) {
            ngMemCpy( dst, src, i_off);
            len -= i_off;
            dst += i_off; /* 28/05/2001 */
            i++;
            if( i > iovcnt) return; /* it should not happen (!) */
            src = iov[i].iov_base;
            i_off = iov[i].iov_len;
        }
        else {
            ngMemCpy( dst, src, len);
            break;
        }
    }
}

