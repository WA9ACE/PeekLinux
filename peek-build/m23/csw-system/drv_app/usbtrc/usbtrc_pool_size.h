/**
 * @file
 *
 * @brief Definition of different memory sizes.
 *
 * This file defines the following information:
 * - the memory bank sizes and their watermark,
 * - the SWE stack size,
 * - the pool size needed (generally the sum of memory bank and stack sizes).
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	4/14/2004	Charles-Hubert BESSON		Create.
 *
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBTRC_POOL_SIZE_H_
#define __USBTRC_POOL_SIZE_H_


#define USBTRC_STACK_SIZE		(1 * 1024)

#if	(LOCOSTO_LITE)
#define USBTRC_MB1_SIZE		(3 * 1024 + 512)
#else
#define USBTRC_MB1_SIZE		(8 * 1024)
#endif // LOCOSTO_LITE

#define USBTRC_POOL_SIZE			(USBTRC_STACK_SIZE + USBTRC_MB1_SIZE)

#endif // __USBTRC_POOL_SIZE_H_
