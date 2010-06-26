/*****************************************************************************
 * $Id: ntoa.c,v 1.2 2001/04/02 14:31:40 rf Exp $
 * $Name: rel_1_3_b_9 $
 *----------------------------------------------------------------------------
 * NexGenIP v1.3
 * Internet address manipulation
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
 * ngInetNTOA()
 *----------------------------------------------------------------------------
 * 25/05/98 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>

/*****************************************************************************
 * ngInetNTOA()
 *****************************************************************************
 * Convert an internet address to an ascii string
 *****************************************************************************
 * Parameters:
 *  addr        internet address.
 *  buffer      destination buffer
 *  buflen      length of buffer
 * Return value: 0 or NG_ENOSPC if buffer too small
 */

int ngInetNTOA( NGuint addr, char *buffer, int buflen)
{
    NGubyte *p;
    char tmpbuf[sizeof("255.255.255.255")];

    p = (NGubyte *) &addr;
    if( ngSPrintf( tmpbuf, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]) > buflen)
        return( NG_ENOSPC);
    ngStrCpy( buffer, tmpbuf);
    return( NG_EOK);
}

