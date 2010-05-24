/**
 * @file	mks_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Vincent Oberle
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	07/08/2003	Vincent Oberle			Extracted from rvf_pool_size.h
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __MKS_POOL_SIZE_H_
#define __MKS_POOL_SIZE_H_


/*
 * Values used in mks_env.h
 */
#if (REMU==1)
#define MKS_STACK_SIZE (1024+500)
#else
#define MKS_STACK_SIZE (500)
#endif
#define MKS_MB1_SIZE   (500)
#define MKS_POOL_SIZE  (MKS_STACK_SIZE + MKS_MB1_SIZE)


#endif /*__MKS_POOL_SIZE_H_*/
