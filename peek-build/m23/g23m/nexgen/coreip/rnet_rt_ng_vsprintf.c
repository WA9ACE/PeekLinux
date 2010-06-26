/*****************************************************************************
 * $Id: vsprintf.c,v 1.2 2001/03/28 18:25:10 rf Exp $
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
 * ngVSPrintf()
 *----------------------------------------------------------------------------
 * 17/10/98 - Regis Feneon
 * 01/12/98 -
 *  Added support for Ethernet and Internet address display
 * 03/02/00 - Adrien Felon
 *  Corrected unsigned number parsing in ngVSPrintf() call, by changing cast
 *  to unsigned pointers to write into variable value (see case ParseNum).
 * 10/02/2000 - Regis Feneon
 *  now just call ngRawPrintf()
 * 02/03/2000 - Adrien Felon
 *  removing ngMemStdOut argument in call to ngRawPrintf()
 *****************************************************************************/

#include <ngos.h>

/*****************************************************************************
 * ngVSPrintf()
 *****************************************************************************
 * Tiny implementation of vsprintf
 *****************************************************************************
 * Parameters:
 *  buf     destination buffer
 *  fmt     format string
 *  args    list of arguments
 * Return value: number of characters written
 */

int ngVSPrintf( char *buf, const char *fmt, NGva_list args)
{
    return( ngRawPrintf( NULL, &buf, fmt, args));
}

