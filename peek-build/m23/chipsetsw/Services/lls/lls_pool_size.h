/**
 * @file	lls_pool_size.h
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

#ifndef __LLS_POOL_SIZE_H_
#define __LLS_POOL_SIZE_H_


/*
 * Values used in lls_env.h
 */
#define LLS_MB1_SIZE  (50)
#define LLS_POOL_SIZE (LLS_MB1_SIZE)


#endif /*__LLS_POOL_SIZE_H_*/
