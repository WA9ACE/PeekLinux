/**
 * @file	usbms_pool_size.h
 *
 * Declarations of:
 * - the memory bank sizes and their watermark
 * - the SWE stack size
 * - the pool size needed (generally the sum of memory bank and stack sizes)
 *
 * @author	Your name here (your_email_here)
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	3/19/2004	Virgile COULANGE		Create.
 *
 * (C) Copyright 2003 by PHILOG, All Rights Reserved
 */

#ifndef __USBMS_POOL_SIZE_H_
#define __USBMS_POOL_SIZE_H_


/*
 * Values used in usbms_env.h
 */
#define USBMS_STACK_SIZE  (10 * 1024)//2048//4096
#define USBMS_MB1_SIZE    (1 * 1024)//2048
#define USBMS_MB2_SIZE    (49 * 1024)	// MAX_COMMAND_SIZE + (1+2) * MAX_BYTE_PER_XFER

#define USBMS_POOL_SIZE		(USBMS_STACK_SIZE + USBMS_MB1_SIZE)
#define USBMS_INT_POOL_SIZE  (USBMS_MB2_SIZE)


#endif /*__USBMS_POOL_SIZE_H_*/
