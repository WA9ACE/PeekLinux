/**
 * @file	hash_pool_size.h
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

#ifndef __HASH_POOL_SIZE_H_
#define __HASH_POOL_SIZE_H_


/*
 * Values used in hash_cfg.h
 */
#define HASH_STACK_SIZE (1024)

#define HASH_MB1_SIZE (1024)

#define HASH_POOL_SIZE  (HASH_STACK_SIZE + HASH_MB1_SIZE)


#endif /*__HASH_POOL_SIZE_H_*/
