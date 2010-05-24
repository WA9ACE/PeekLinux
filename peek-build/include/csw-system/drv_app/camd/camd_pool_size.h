/**
 * @file	camd_pool_size.h
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

#ifndef __CAMD_POOL_SIZE_H_
#define __CAMD_POOL_SIZE_H_


/*
 * Values used in camd_cfg.h
 */
#define CAMD_STACK_SIZE (1024)

#define CAMD_MB1_SIZE (512)
#define CAMD_MB2_SIZE (1024 + 512)

#define CAMD_POOL_SIZE      (CAMD_STACK_SIZE + CAMD_MB1_SIZE)
#define CAMD_INT_POOL_SIZE  (CAMD_MB2_SIZE)


#endif /*__CAMD_POOL_SIZE_H_*/
