/*****************************************************************************
 * $Id: atol.c,v 1.2 2001/03/28 18:25:10 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Standart Librairies Replacement Functions
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
 * ngATOL()
 *----------------------------------------------------------------------------
 * 21/08/2000 - Regis Feneon
 *****************************************************************************/

#include <ngos.h>

/*****************************************************************************
 * ngATOL()
 *****************************************************************************
 * Convert a sring to long integer
 *****************************************************************************
 * Parameters:
 *  str     pointer to string to convert
 * Return value: converted value
 */

long ngATOL( const char *str)
{
    const char *p;
    int c, neg;
    long val;

    val = 0;

    /* skip spaces and get +/- sign if any */
    p = str;
    do {
        c = (unsigned char) *p++;
    }
    while( c == ' ');
    if( c == '-') {
        neg = 1;
        c = (unsigned char) *p++;
    }
    else {
        neg = 0;
        if( c == '+') c = (unsigned char) *p++;
    }

    /* get digits */
    while( (c >= '0') && (c <= '9')) {
        c -= '0';
        val *= 10;
        val += c;
        c = (unsigned char) *p++;
    }

    /* negative value */
    if( neg) val = -val;

    return( val);
}

