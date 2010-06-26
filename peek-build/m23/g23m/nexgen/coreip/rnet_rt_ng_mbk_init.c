/*****************************************************************************
 * $Id: mbk_init.c,v 1.2 2001/03/28 18:25:10 rf Exp $
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
 * ngMemBlkInit()
 *----------------------------------------------------------------------------
 * 21/08/2000 - Regis Feneon
 * 24/10/2000 -
 *  corrected blk_size (!)
 *****************************************************************************/

#include <ngos.h>
#include <ngmemblk.h>

/*****************************************************************************
 * ngMemBlkInit()
 *****************************************************************************
 * Initialize a pool of memory blocks
 *****************************************************************************
 * Parameters:
 *  memp        pointer to the memory pool description structure
 *  addr        pointer to the pool memory
 *  blk_nb      number of blocks
 *  blk_size    size of one block
 * Return value:
 *  NG_EINVAL   blk_size is invalid
 *  NG_EOK      no error
 */

int ngMemBlkInit( NGmemblk *memp, void *addr, int blk_nb, int blk_size)
{
    int i;
    NGubyte *ptr;

    if( (unsigned)blk_size < sizeof( void *)) return( NG_EINVAL); /* blocks too small */

    /* blocks must be aligned on processor word boundaries */
#ifdef NG_ALIGN16
    if( blk_size & 1) return( NG_EINVAL);
#endif
#ifdef NG_ALIGN32
    if( blk_size & 3) return( NG_EINVAL);
#endif

    /* initialize memory pool */
    if( blk_nb > 0) {
        ptr = addr;
        for( i=0; i<(blk_nb-1); i++) {
            /* points to next block */
            *((void **) ptr) = (void *) (ptr + blk_size);
            ptr = (NGubyte *) *((void **) ptr);
        }
        *((void **) ptr) = NULL;
    }
    else {
        /* empty memory pool */
        blk_nb = 0;
        addr = NULL;
    }

    /* initialize memp structure */
    memp->mem_total = blk_nb;
    memp->mem_nb = blk_nb;
    memp->mem_list = addr;
    return( NG_EOK);
}

