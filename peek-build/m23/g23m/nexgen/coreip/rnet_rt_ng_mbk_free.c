/*****************************************************************************
 * $Id: mbk_free.c,v 1.2 2001/03/28 18:25:10 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Memory Blocks Management
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
 * ngMemBlkFree()
 *----------------------------------------------------------------------------
 * 21/08/2000 - Regis Feneon
 *****************************************************************************/

#include <ngos.h>
#include <ngmemblk.h>

/*****************************************************************************
 * ngMemBlkFree()
 *****************************************************************************
 * Release a memory block
 * Interrupts are locked during operation
 *****************************************************************************
 * Parameters:
 *  memp        pointer to the memory pool description structure
 *  blkp        pointer to the block to release
 * Return value:
 *  none
 */

void ngMemBlkFree( NGmemblk *memp, void *blkp)
{
    int ictl;

    /* lock interrupts */
    ictl = ngOSIntrCtl( NG_INTRCTL_DISABLE);
    /* release memory block */
    NG_MEMBLK_FREE( memp, blkp);
    /* unlock interrupts */
    ngOSIntrCtl( ictl);
}

