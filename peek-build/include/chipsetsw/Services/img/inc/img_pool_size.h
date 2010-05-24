/**
 * @file	img_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	J Raghuram Karthik
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	11/03/2006	Anandhi Ramesh			Creation
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __IMG_POOL_SIZE_H_
#define __IMG_POOL_SIZE_H_


/*
 * Values used in Stack & Memory Banks
 */
#ifdef CORE_TRACE
    #define IMG_STACK_SIZE  8000
#else
    #define IMG_STACK_SIZE  3150
#endif

#endif /*__SSL_POOL_SIZE_H_*/
