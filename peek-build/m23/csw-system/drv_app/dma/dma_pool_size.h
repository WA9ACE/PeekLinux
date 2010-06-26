/**
 * @file	dma_pool_size.h
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

#ifndef __DMA_POOL_SIZE_H_
#define __DMA_POOL_SIZE_H_


/*
 * Values used in dma_cfg.h
 */
#if (REMU==1)
#define DMA_STACK_SIZE (2048)
#else
#define DMA_STACK_SIZE (512)
#endif

#define DMA_MB1_SIZE (4096)  //old 2048

#define DMA_POOL_SIZE      (DMA_STACK_SIZE)
#define DMA_INT_POOL_SIZE  (DMA_MB1_SIZE)


#endif /*__DMA_POOL_SIZE_H_*/
