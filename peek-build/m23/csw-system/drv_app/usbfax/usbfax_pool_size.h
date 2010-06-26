/**
 * @file	usbfax_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	19/03/2004	
 */

#ifndef __USBFAX_POOL_SIZE_H_
#define __USBFAX_POOL_SIZE_H_


/*
 * Values used in usbfax_cfg.h
 */
#define USBFAX_STACK_SIZE (3 * 1024)

#define USBFAX_MB1_SIZE (8 * 1024)

#define USBFAX_POOL_SIZE  (USBFAX_STACK_SIZE + USBFAX_MB1_SIZE)


#endif /*__USBFAX_POOL_SIZE_H_*/
