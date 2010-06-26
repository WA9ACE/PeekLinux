/**
 * @file	cry_cfg.h
 *
 * Configuration definitions for the CRY instance.
 *
 * @author	 ()
 * @version 0.1
 */

/*
 * History:
 *
 *	Date       	Author					Modification
 *	-------------------------------------------------------------------
 *	7/14/2003	 ()		Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __CRY_CFG_H_
#define __CRY_CFG_H_


#include "rv/rv_general.h"       /* General Riviera definitions. */
#include "cry/cry_pool_size.h"   /* Stack & Memory Bank sizes definitions */

/**
 * Sample value.
 */
#define CRY_MAX_BLOCK_SIZE 64


/**
 * @name Memory bank size and stack size
 *
 * Memory bank size and watermark.
 * Wished task stack in bytes.
 */
/*@{*/
#define CRY_MB_PRIM_SIZE            CRY_MB1_SIZE
#define CRY_MB_PRIM_WATERMARK       (CRY_MB_PRIM_SIZE - 128)
/*@}*/

/** 
 * Wished priority of the host task for the SWE.
 *
 * During development, put the hardcoded value here.
 * After integration, the value should be in rvm_priorities.h
 */
#ifdef RVM_CRY_TASK_PRIORITY
#define CRY_TASK_PRIORITY               RVM_CRY_TASK_PRIORITY	
#else
#define CRY_TASK_PRIORITY               170
#endif

/* Used for software version build */
#define CRY_MAJOR 0
#define CRY_MINOR 2
#define CRY_BUILD 0

#define CRY_ENABLE_DMA
#define CRY_DMA_THRESHOLD (128) /* bytes */

#endif /* __CRY_CFG_H_ */
