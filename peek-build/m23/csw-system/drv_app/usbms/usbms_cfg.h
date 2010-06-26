/**
 * @file	usbms_cfg.h
 *
 * Configuration definitions for the USBMS instance.
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
 * (C) Copyright 2004 by PHILOG, All Rights Reserved
 */

#ifndef __USBMS_CFG_H_
#define __USBMS_CFG_H_


#include "rv/rv_general.h"		/* General Riviera definitions. */
#include "usbms/usbms_pool_size.h"

/**
 * Sample value.
 */
#define USBMS_SAMPLE_VALUE	            10


/**
 * @name Mem bank
 *
  * Memory bank size and watermark.
 * During development, it is better to put the value here (when the
 * test are activated).
 * At integration, put the value in rvf_pool_size.h
*/

#define USBMS_MB_PRIM_SIZE                (USBMS_MB1_SIZE)
#define USBMS_MB_PRIM_WATERMARK           (USBMS_MB_PRIM_SIZE - 256)

#define USBMS_MB_SEC_SIZE                (USBMS_MB2_SIZE)
#define USBMS_MB_SEC_WATERMARK           (USBMS_MB_SEC_SIZE - 256)

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */

#ifdef RVM_USBMS_TASK_PRIORITY
#define USBMS_TASK_PRIORITY               RVM_USBMS_TASK_PRIORITY
#else
#define USBMS_TASK_PRIORITY               73
#endif


#endif /* __USBMS_CFG_H_ */
