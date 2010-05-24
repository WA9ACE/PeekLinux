/**
 * @file	tty_pool_size.h
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

#ifndef __TTY_POOL_SIZE_H_
#define __TTY_POOL_SIZE_H_


/*
 * Values used in tty_env.h
 */
#if (LOCOSTO_LITE)
#define TTY_STACK_SIZE (2040)
#define TTY_MB1_SIZE   (512)
#else
#define TTY_STACK_SIZE (3088)
#define TTY_MB1_SIZE   (8200)
#endif
#define TTY_POOL_SIZE  (TTY_STACK_SIZE + TTY_MB1_SIZE)


#endif /*__TTY_POOL_SIZE_H_*/
