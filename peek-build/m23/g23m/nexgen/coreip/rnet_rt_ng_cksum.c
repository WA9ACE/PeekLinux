/*****************************************************************************
 * $Id: cksum.c,v 1.3 2001/11/20 14:36:07 rf Exp $
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
 * ngInetChecksum()
 *----------------------------------------------------------------------------
 * 28/05/99 - Regis Feneon
 *****************************************************************************/

#include <ngip.h>

/*****************************************************************************
 * ngInetChecksum()
 *****************************************************************************
 * Standart IP checksum calculation routine
 *****************************************************************************
 * Parameters:
 *  ptr         pointer to buffer
 *  len         size of buffer
 * Return value: checksum value
 */

NGushort ngInetChecksum( void *ptr, int len)
{
    /* just call internal routine in_cksum() */
    return( ngIpCksum( ptr, len, NULL, 0));
}

