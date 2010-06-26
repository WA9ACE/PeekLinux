/*****************************************************************************
 * $Id: sprintf.c,v 1.2 2001/03/28 18:25:10 rf Exp $
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
 * ngSPrintf()
 *----------------------------------------------------------------------------
 * 11/11/98 - Regis Feneon
 * 10/02/2000 -
 *  now call ngRawPrintf()
 * 02/03/2000 - Adrien Felon
 *  removing ngMemStdOut argument in call to ngRawPrintf()
 *****************************************************************************/

#include <ngos.h>

/*****************************************************************************
 * ngSPrintf()
 *****************************************************************************
 * Write a formated output into a string
 *****************************************************************************
 * Parameters:
 *  buf     destination buffer
 *  format  format string (see ngRawPrintf())
 * Return value: the number of characters written
 */

int ngSPrintf( char *buf, const char *fmt,...)
{
    NGva_list ap;
    int ret;

    NG_VA_START( ap, fmt);
    ret = ngRawPrintf( NULL, &buf, fmt, ap);
    NG_VA_END( ap);
    return( ret);
}

