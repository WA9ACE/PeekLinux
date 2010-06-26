/**
 * @file	cry_pool_size.h
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
 *	07/08/2003	Candice Bazanegue			Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __CRY_POOL_SIZE_H_
#define __CRY_POOL_SIZE_H_


/*
 * Values used in cry_cfg.h
 */
#define CRY_STACK_SIZE (512)

#define CRY_MB1_SIZE (512)

#define CRY_POOL_SIZE  (CRY_STACK_SIZE + CRY_MB1_SIZE)


#endif /*__CRY_POOL_SIZE_H_*/
