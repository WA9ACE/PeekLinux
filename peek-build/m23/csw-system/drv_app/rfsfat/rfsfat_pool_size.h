/**
 * @file	rfsfat_pool_size.h
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

#ifndef __RFSFAT_POOL_SIZE_H_
#define __RFSFAT_POOL_SIZE_H_


 /*
  * Values used in rfsfat_cfg.h
  */
#define RFSFAT_STACK_SIZE        (4096)
#define RFSFAT_HISR_STACK_SIZE   (1024)

#ifdef ICT_SPEEDUP_RFSFAT2
#define RFSFAT_COMMON_USE        (3000+10240)
#else //ifdef ICT_SPEEDUP_RFSFAT2
#define RFSFAT_COMMON_USE        (3000)
#endif //ifdef ICT_SPEEDUP_RFSFAT2
#define RFSFAT_FORMAT_USE        (9000)
#define RFSFAT_MB1_SIZE 					(RFSFAT_COMMON_USE + RFSFAT_FORMAT_USE)

#define RFSFAT_POOL_SIZE  (RFSFAT_STACK_SIZE + RFSFAT_MB1_SIZE)


#endif /*__RFSFAT_POOL_SIZE_H_*/
