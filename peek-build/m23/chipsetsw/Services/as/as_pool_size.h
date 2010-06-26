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
 *	Date       	Author			Modification
 *	-------------------------------------------------------------------
 *	1/25/2005	f-maria@ti.com		Create.
 *
 * (C) Copyright 2005 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __AS_POOL_SIZE_H_
#define __AS_POOL_SIZE_H_


/// The size of the stack required to run the AS SWE.
#define AS_STACK_SIZE        (3072)

/// The size of the external memory bank.
#define AS_MB_EXT_SIZE       (512)

/// The watermark of the external memory bank.
#define AS_MB_EXT_WATERMARK  (AS_MB_EXT_SIZE - 32)

/// The total of external memory required by the AS SWE.
#define AS_POOL_SIZE         (AS_STACK_SIZE + AS_MB_EXT_SIZE)

/// The total of internal memory required by the AS SWE.
#define AS_INT_POOL_SIZE     0

#endif // __AS_POOL_SIZE_H_
