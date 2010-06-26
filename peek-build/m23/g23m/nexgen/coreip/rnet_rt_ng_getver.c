/*****************************************************************************
 * $Id: getver.c,v 1.2 2001/03/28 18:25:10 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Stack and Modules Version Information
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
 * ngGetVersionString()
 *----------------------------------------------------------------------------
 * 12/08/99 - Regis Feneon
 *****************************************************************************/

#include <ngos.h>

extern char ngVer_str[];

/*****************************************************************************
 * ngGetVersionString()
 *****************************************************************************
 * Return the version and copyright string of NexGenIP modules
 *****************************************************************************
 * Parameters:
 *  none
 * Return value: pointer to version string
 */

char *ngGetVersionString( void)
{
    return( ngVer_str);
}

