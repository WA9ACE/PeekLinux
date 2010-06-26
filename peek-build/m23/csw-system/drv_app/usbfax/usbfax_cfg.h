/**
 * @file	usbfax_cfg.h
 *
 * Configuration definitions for the USBFAX instance.
 * 
 * @author
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	3/17/2004							Create.
 *
 */

#ifndef __USBFAX_CFG_H_
#define __USBFAX_CFG_H_


#include "rv_general.h"		/* General Riviera definitions. */
#include "usbfax/usbfax_pool_size.h"   /* Stack & Memory Bank sizes definitions */

#define USBFAX_BUILD_NR					0
#define USBFAX_MINOR_SW_VERSION_NR		1
#define USBFAX_MAJOR_SW_VERSION_NR		0

#define	USBFAX_NUM_DEVICES		1	/* Number of CDC devices			*/

/* Queues requests specification */
#define	USBFAX_MAX_REQUESTS		2	/* depth of the transmission queue	*/

#define	USBFAX_NUM_DATA_PACKET	32  /* number of packet of 64 bytes to tranmit 3*1024 bytes */

T_RVM_RETURN	usbfax_init_cdc_devices();

/**
 * @name Mem bank
 *
 * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
 */
/*@{*/

#define USBFAX_MB_PRIM_SIZE                (USBFAX_MB1_SIZE)
#define USBFAX_MB_PRIM_WATERMARK           (USBFAX_MB_PRIM_SIZE - 128)

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_USBFAX_TASK_PRIORITY
#define USBFAX_TASK_PRIORITY               RVM_USBFAX_TASK_PRIORITY	
#else
#define USBFAX_TASK_PRIORITY               81 //old 81 then 71
#endif


#endif /* __USBFAX_CFG_H_ */
