/**
 * @file	rfs_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Candice Bazanegue
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	07/08/2003	Candice Bazanegue		Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __RFS_POOL_SIZE_H_
#define __RFS_POOL_SIZE_H_


/*
 * Values used in rfs_cfg.h
 */
#if (REMU==1)
#define RFS_STACK_SIZE (2048+512+512)
#else
#define RFS_STACK_SIZE (2048+512)
#endif
#define RFS_MB1_SIZE (2048+1024)

#define RFS_POOL_SIZE  (RFS_STACK_SIZE + RFS_MB1_SIZE)


#endif /*__RFS_POOL_SIZE_H_*/
