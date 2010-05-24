/**
 * @file	usb_pool_size.h
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

#ifndef __USB_POOL_SIZE_H_
#define __USB_POOL_SIZE_H_


/*
 * Values used in usb_cfg.h
 */
#define USB_STACK_SIZE (1 * 1024)

#if (LOCOSTO_LITE)
#define USB_MB1_SIZE (1 * 1024)
#else
#define USB_MB1_SIZE (2 * 1024)
#endif // LOCOSTO_LITE

#define USB_POOL_SIZE  (USB_STACK_SIZE + USB_MB1_SIZE)


#endif /*__USB_POOL_SIZE_H_*/
