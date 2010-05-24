/**
 * @file	ssl_pool_size.h
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
 *	11/03/2006	J Raghuram Karthik			Creation
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __SSL_POOL_SIZE_H_
#define __SSL_POOL_SIZE_H_


/*
 * Values used in Stack & Memory Banks
 */
#define SSL_STACK_SIZE   (1024)

#define SSL_POOL_SIZE    SSL_STACK_SIZE


#endif /*__SSL_POOL_SIZE_H_*/
